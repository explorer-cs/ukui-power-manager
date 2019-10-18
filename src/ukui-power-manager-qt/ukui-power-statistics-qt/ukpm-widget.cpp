#include "ukpm-widget.h"
#include "customtype.h"
#include "sys/time.h"
#include "statistics-common.h"

#define GPM_HISTORY_RATE_TEXT			"Rate"
#define GPM_HISTORY_CHARGE_TEXT			"Charge"
#define GPM_HISTORY_TIME_FULL_TEXT		"Time to full"
#define GPM_HISTORY_TIME_EMPTY_TEXT		"Time to empty"

#define GPM_HISTORY_RATE_VALUE			"rate"
#define GPM_HISTORY_CHARGE_VALUE		"charge"
#define GPM_HISTORY_TIME_FULL_VALUE		"time-full"
#define GPM_HISTORY_TIME_EMPTY_VALUE		"time-empty"

#define GPM_HISTORY_MINUTE_TEXT			"10 minutes"
#define GPM_HISTORY_HOUR_TEXT			"2 hours"
#define GPM_HISTORY_HOURS_TEXT			"6 hours"
#define GPM_HISTORY_DAY_TEXT			"1 day"
#define GPM_HISTORY_WEEK_TEXT			"1 week"

#define GPM_HISTORY_MINUTE_VALUE		10*60
#define GPM_HISTORY_HOUR_VALUE			2*60*60
#define GPM_HISTORY_HOURS_VALUE			6*60*60
#define GPM_HISTORY_DAY_VALUE			24*60*60
#define GPM_HISTORY_WEEK_VALUE			7*24*60*60

#define GPM_STATS_CHARGE_DATA_VALUE		"charge-data"
#define GPM_STATS_CHARGE_ACCURACY_VALUE		"charge-accuracy"
#define GPM_STATS_DISCHARGE_DATA_VALUE		"discharge-data"
#define GPM_STATS_DISCHARGE_ACCURACY_VALUE	"discharge-accuracy"

UkpmWidget::UkpmWidget(QWidget *parent)
    : QWidget(parent)
{
    settings = new QGSettings(GPM_SETTINGS_SCHEMA);

    initUI();

    registerCustomType();

    getDevices();

    connectSlots();

}

UkpmWidget::~UkpmWidget()
{
    for(auto iter = listItem.begin(); iter!= listItem.end(); iter++)
    {
        listItem.erase(iter);
        delete iter.value();
    }
}

QString
UkpmWidget::device_kind_to_localised_text (UpDeviceKind kind, uint number)
{
    Q_UNUSED(number);
    QString text = NULL;
    switch (kind) {
    case UP_DEVICE_KIND_LINE_POWER:
        /* TRANSLATORS: system power cord */
        text = tr ("AC adapter");
        break;
    case UP_DEVICE_KIND_BATTERY:
        /* TRANSLATORS: laptop primary battery */
        text = tr ("Laptop battery");
        break;
    case UP_DEVICE_KIND_UPS:
        /* TRANSLATORS: battery-backed AC power source */
        text = tr ("UPS");
        break;
    case UP_DEVICE_KIND_MONITOR:
        /* TRANSLATORS: a monitor is a device to measure voltage and current */
        text = tr ("Monitor");
        break;
    case UP_DEVICE_KIND_MOUSE:
        /* TRANSLATORS: wireless mice with internal batteries */
        text = tr ("Mouse");
        break;
    case UP_DEVICE_KIND_KEYBOARD:
        /* TRANSLATORS: wireless keyboard with internal battery */
        text = tr ("Keyboard");
        break;
    case UP_DEVICE_KIND_PDA:
        /* TRANSLATORS: portable device */
        text = tr ("PDA");
        break;
    case UP_DEVICE_KIND_PHONE:
        /* TRANSLATORS: cell phone (mobile...) */
        text = tr ("Cell phone");
        break;
    case UP_DEVICE_KIND_MEDIA_PLAYER:
        /* TRANSLATORS: media player, mp3 etc */
        text = tr ("Media player");
        break;
    case UP_DEVICE_KIND_TABLET:
        /* TRANSLATORS: tablet device */
        text = tr ("Tablet");
        break;
    case UP_DEVICE_KIND_COMPUTER:
        /* TRANSLATORS: tablet device */
        text = tr ("Computer");
        break;
    default:
        text = up_device_kind_to_string (kind);
    }
    return text;
}

int UkpmWidget::parseArguments()
{
    int index = 0;
    QStringList arguments = QApplication::arguments();
    qDebug()<< "arguments size is:"<<arguments.size();
    if(arguments.size() < 3)
        return index;
    QString dev = arguments.at(2);
    int size = deviceNames.count();
    for(int i = 0; i< size; i++)
        if(deviceNames.at(i).path() == dev)
            index = i;
    qDebug()<< dev << " of index=" << index;
    return index;
}

void UkpmWidget::getDevices()
{
    QListWidgetItem *item;
    QString kind,vendor,model,label;
    int kindEnum = 0;
    QDBusMessage msg = QDBusMessage::createMethodCall(DBUS_SERVICE,DBUS_OBJECT,
            "org.freedesktop.UPower","EnumerateDevices");
    QDBusMessage res = QDBusConnection::systemBus().call(msg);

    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        dbusArg >> deviceNames;
        qDebug()<<"get devices size!"<<deviceNames.size();
    }
    else {
        qDebug()<<"No response!";
    }
    int len = deviceNames.size();
    for(int i = 0; i < len; i++)
    {
        if(deviceNames.at(i).path().contains("battery"))
            batterySvr = deviceNames.at(i).path();
        else if(deviceNames.at(i).path().contains("line_power"))
            acSvr = deviceNames.at(i).path();
        QDBusMessage msg = QDBusMessage::createMethodCall(DBUS_SERVICE,deviceNames.at(i).path(),
                DBUS_INTERFACE,"GetAll");
        msg << DBUS_INTERFACE_PARAM;
        QDBusMessage res = QDBusConnection::systemBus().call(msg);

        if(res.type() == QDBusMessage::ReplyMessage)
        {
            const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
            QMap<QString,QVariant> map;
            dbusArg >> map;
            kind = map.value(QString("kind")).toString();
            if(kind.length() ==0)
                kind = map.value(QString("Type")).toString();
            kindEnum = kind.toInt();
            QString icon = up_device_kind_to_string((UpDeviceKind)kindEnum);
            vendor = map.value(QString("Vendor")).toString();
            model = map.value(QString("Model")).toString();
            if(vendor.length() != 0 && model.length() != 0)
                label = vendor + " " + model;
            else
                label =device_kind_to_localised_text((UpDeviceKind)kindEnum,1);
            if(kindEnum == UP_DEVICE_KIND_LINE_POWER || kindEnum == UP_DEVICE_KIND_BATTERY || kindEnum == UP_DEVICE_KIND_COMPUTER)
            {
                item = new QListWidgetItem(QIcon(":/"+icon),label);
                listItem.insert(deviceNames.at(i),item);
                listWidget->insertItem(i,item);
            }

            DEV dev;
            dev.Type = up_device_kind_to_string ((UpDeviceKind)map.value(QString("Type")).toInt());
            dev.Model = map.value(QString("Model")).toString();
            dev.Device = map.value(QString("NativePath")).toString();
            dev.Vendor = map.value(QString("Vendor")).toString();
            dev.Capacity = QString::number(map.value(QString("Capacity")).toDouble(), 'f', 1) + "%";
            dev.Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
            dev.EnergyEmpty= QString::number(map.value(QString("EnergyEmpty")).toDouble(), 'f', 1)+ " Wh";
            dev.EnergyFull = QString::number(map.value(QString("EnergyFull")).toDouble(), 'f', 1)+ " Wh";
            dev.EnergyFullDesign = QString::number(map.value(QString("EnergyFullDesign")).toDouble(), 'f', 1) + " Wh";
            dev.EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
            dev.IsPresent = boolToString(map.value(QString("IsPresent")).toBool());
            dev.IsRechargeable = boolToString(map.value(QString("IsRechargeable")).toBool());
            dev.PowerSupply = boolToString(map.value(QString("PowerSupply")).toBool());
            dev.Percentage = QString::number(map.value(QString("Percentage")).toDouble(), 'f', 1)+"%";
            dev.Online = boolToString(map.value(QString("Online")).toBool());
            struct timeval tv;
            uint tim;
            gettimeofday(&tv,NULL);
            tim = tv.tv_sec - map.value(QString("UpdateTime")).toLongLong();
            btrDetailData.Refresh = getSufix(tim,'s');

            flag = map.value(QString("State")).toLongLong();
            switch (flag) {
            case 1:
                dev.State = tr("Charging");
                break;
            case 2:
                dev.State = tr("Discharging");
                break;
            case 3:
                dev.State = tr("Empty");
                break;
            case 4:
                dev.State = tr("Charged");
                break;
            default:
                break;
            }
            calcTime(dev.TimeToEmpty, map.value(QString("TimeToEmpty")).toLongLong());
            calcTime(dev.TimeToFull, map.value(QString("TimeToFull")).toLongLong());
            dev.Voltage = QString::number(map.value(QString("Voltage")).toDouble(), 'f', 1) + " V";

            devices.push_back(dev);
        }
    }
    if(!batterySvr.isEmpty())
    {
        memcpy(&btrDetailData,&devices.at(0),sizeof(DEV));
    	setupBtrUI();
    }
    if(!acSvr.isEmpty())
    {
        memcpy(&dcDetailData,&devices.at(1),sizeof(DEV));
    	setupDcUI();
    }

}


void UkpmWidget::initBtrDetail(QString btr)
{

    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.UPower",btr,
            "org.freedesktop.DBus.Properties","GetAll");
    msg << "org.freedesktop.UPower.Device";
    QDBusMessage res = QDBusConnection::systemBus().call(msg);

    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        QMap<QString,QVariant> map;
        dbusArg >> map;

        btrDetailData.Type = (map.value(QString("Type")).toInt()==2) ? tr("Notebook battery") : tr("other");
        btrDetailData.Model = map.value(QString("Model")).toString();
        btrDetailData.Device = btr.section('/',-1);
        btrDetailData.Vendor = map.value(QString("Vendor")).toString();
        btrDetailData.Capacity = QString::number(map.value(QString("Capacity")).toDouble(), 'f', 1) + "%";
        btrDetailData.Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
        btrDetailData.EnergyEmpty= QString::number(map.value(QString("EnergyEmpty")).toDouble(), 'f', 1)+ " Wh";
        btrDetailData.EnergyFull = QString::number(map.value(QString("EnergyFull")).toDouble(), 'f', 1)+ " Wh";
        btrDetailData.EnergyFullDesign = QString::number(map.value(QString("EnergyFullDesign")).toDouble(), 'f', 1) + " Wh";
        btrDetailData.EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
        btrDetailData.IsPresent = (map.value(QString("IsPresent")).toBool()) ? tr("Yes") : tr("No");
        btrDetailData.IsRechargeable = (map.value(QString("IsRechargeable")).toBool()) ? tr("Yes") : tr("No");
        btrDetailData.PowerSupply = (map.value(QString("PowerSupply")).toBool()) ? tr("Yes") : tr("No");
        btrDetailData.Percentage = QString::number(map.value(QString("Percentage")).toDouble(), 'f', 1)+"%";
        flag = map.value(QString("State")).toLongLong();

        switch (flag) {
        case 1:
            dcDetailData.Online = tr("Yes");
            btrDetailData.State = tr("Charging");
            break;
        case 2:
            dcDetailData.Online = tr("No");
            btrDetailData.State = tr("Discharging");
            break;
        case 3:
            btrDetailData.State = tr("Empty");
            break;
        case 4:
            btrDetailData.State = tr("Charged");
            break;
        default:
            break;
        }

        calcTime(btrDetailData.TimeToEmpty, map.value(QString("TimeToEmpty")).toLongLong());
        calcTime(btrDetailData.TimeToFull, map.value(QString("TimeToFull")).toLongLong());
        btrDetailData.Voltage = QString::number(map.value(QString("Voltage")).toDouble(), 'f', 1) + " V";
    }
    else {
        printf("No response!\n");
    }
}


void UkpmWidget::onUSBDeviceHotPlug(int drvid, int action, int devNumNow)
{
    qDebug() << "device"<< (action > 0 ? "insert:" : "pull out:");
    qDebug() << "id:" << drvid << devNumNow;
}

void UkpmWidget::calcTime(QString &attr, uint time)
{
//    qlonglong time = map.value(QString("TimeToFull")).toLongLong();
    if(time < 60)
    {
//        attr = QString::number(time) + tr(" s");
        attr = getSufix(time,'s');
        return;
    }
    time /= 60;
    if(time < 60)
    {
//        attr = QString::number(time) + tr(" m");
        attr = getSufix(time,'m');
        return;
    }
    qreal hour = time / 60.0;
    {
//        attr = QString::number(hour,'f', 1) + tr(" h");
        attr = getSufix(hour,'h');
        return;
    }
}

void UkpmWidget::putAttributes(QMap<QString,QVariant>& map)
{
    if(map.contains("TimeToFull"))
    {
        calcTime(btrDetailData.TimeToFull,map.value(QString("TimeToFull")).toLongLong());
    }
    if(map.contains("TimeToEmpty"))
        calcTime(btrDetailData.TimeToEmpty, map.value(QString("TimeToEmpty")).toLongLong());
    if(map.contains("EnergyRate"))
        btrDetailData.EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
    if(map.contains("Energy"))
        btrDetailData.Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
    if(map.contains("Voltage"))
        btrDetailData.Voltage = QString::number(map.value(QString("Voltage")).toDouble(), 'f', 1) + " V";
    if(map.contains("UpdateTime"))
    {
        struct timeval tv;
        uint tim;
        gettimeofday(&tv,NULL);
        tim = tv.tv_sec - map.value(QString("UpdateTime")).toLongLong();
        btrDetailData.Refresh = getSufix(tim,'s');
    }

    if(map.contains("State"))
    {
        flag = map.value(QString("State")).toLongLong();

        switch (flag) {
        case 1:
            btrDetailData.State = tr("Charging");
            break;
        case 2:
            btrDetailData.State = tr("Discharging");
            break;
        case 3:
            btrDetailData.State = tr("Empty");
            break;
        case 4:
            btrDetailData.State = tr("Charged");
            break;
        default:
            break;
        }

    }
    if(map.contains("Percentage"))
    {
        btrDetailData.Percentage = QString::number(map.value(QString("Percentage")).toDouble(), 'f', 1)+"%";
    }

    if(map.contains("PowerSupply"))
        btrDetailData.PowerSupply = boolToString(map.value(QString("PowerSupply")).toBool());

    detailBTRTable->item(1,1)->setText(btrDetailData.Type);
    detailBTRTable->item(2,1)->setText(btrDetailData.Vendor);
    detailBTRTable->item(3,1)->setText(btrDetailData.Model);
    detailBTRTable->item(4,1)->setText(btrDetailData.PowerSupply);
    detailBTRTable->item(5,1)->setText(btrDetailData.Refresh);
    detailBTRTable->item(6,1)->setText(btrDetailData.IsPresent);
    detailBTRTable->item(7,1)->setText(btrDetailData.IsRechargeable);
    detailBTRTable->item(8,1)->setText(btrDetailData.State);
    detailBTRTable->item(9,1)->setText(btrDetailData.Energy);
    detailBTRTable->item(10,1)->setText(btrDetailData.EnergyFull);
    detailBTRTable->item(11,1)->setText(btrDetailData.EnergyFullDesign);
    detailBTRTable->item(12,1)->setText(btrDetailData.EnergyRate);
    detailBTRTable->item(13,1)->setText(btrDetailData.Voltage);
    detailBTRTable->item(14,1)->setText(btrDetailData.TimeToFull);
    detailBTRTable->item(15,1)->setText(btrDetailData.TimeToEmpty);
    detailBTRTable->item(16,1)->setText(btrDetailData.Percentage);
    detailBTRTable->item(17,1)->setText(btrDetailData.Capacity);
}


void UkpmWidget::onShow()
{
    QDesktopWidget *deskdop = QApplication::desktop();
    move((deskdop->width() - this->width())/2, (deskdop->height() - this->height())/2);
    this->show();
}

void UkpmWidget::onActivatedIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    {
        QPoint post;
        post.setX(-menu->sizeHint().width()/2 + QCursor::pos().x());
        post.setY(QCursor::pos().y());
        menu->exec(post);
    }
        break;

    default:
        break;
    }

}

void UkpmWidget::addNewUI(QDBusObjectPath &path)
{
    QTabWidget *tabWidgetDC = new QTabWidget();
    QWidget *DetailDc = new QWidget();
    tabWidgetDC->addTab(DetailDc,QString());
    tabWidgetDC->setTabText(0,tr("Detail"));
    stackedWidget->addWidget(tabWidgetDC);
    widgetItem.insert(path,tabWidgetDC);

    detailDcTable = new QTableWidget(4,2,DetailDc);
    QStringList strList;
    strList << tr("attribute") << tr("value");
    detailDcTable->setHorizontalHeaderLabels(strList);

    detailDcTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    detailDcTable->setItem(0,0,new QTableWidgetItem(tr("Device")));
    detailDcTable->setItem(1,0,new QTableWidgetItem(tr("Type")));
    detailDcTable->setItem(2,0,new QTableWidgetItem(tr("PowerSupply")));
    detailDcTable->setItem(3,0,new QTableWidgetItem(tr("Online")));

    detailDcTable->setItem(0,1,new QTableWidgetItem(dcDetailData.Device));
    detailDcTable->setItem(1,1,new QTableWidgetItem(dcDetailData.Type));
    detailDcTable->setItem(2,1,new QTableWidgetItem(dcDetailData.PowerSupply));
    detailDcTable->setItem(3,1,new QTableWidgetItem(dcDetailData.Online));
    detailDcTable->verticalHeader()->setVisible(false);
    detailDcTable->horizontalHeader()->setStretchLastSection(true);

    QVBoxLayout *detailDcLayout = new QVBoxLayout;
    detailDcLayout->addWidget(detailDcTable);
    DetailDc->setLayout(detailDcLayout);
}

void UkpmWidget::setupDcUI()
{
    tabWidgetDC = new QTabWidget();
    QWidget *DetailDc = new QWidget();
    tabWidgetDC->addTab(DetailDc,QString());
    tabWidgetDC->setTabText(0,tr("Detail"));
    stackedWidget->addWidget(tabWidgetDC);
    detailDcTable = new QTableWidget(4,2,DetailDc);
    QStringList strList;
    strList << tr("attribute") << tr("value");
    detailDcTable->setHorizontalHeaderLabels(strList);

    detailDcTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    detailDcTable->setItem(0,0,new QTableWidgetItem(tr("Device")));
    detailDcTable->setItem(1,0,new QTableWidgetItem(tr("Type")));
    detailDcTable->setItem(2,0,new QTableWidgetItem(tr("PowerSupply")));
    detailDcTable->setItem(3,0,new QTableWidgetItem(tr("Online")));

    detailDcTable->setItem(0,1,new QTableWidgetItem(dcDetailData.Device));
    detailDcTable->setItem(1,1,new QTableWidgetItem(dcDetailData.Type));
    detailDcTable->setItem(2,1,new QTableWidgetItem(dcDetailData.PowerSupply));
    detailDcTable->setItem(3,1,new QTableWidgetItem(dcDetailData.Online));

    detailDcTable->verticalHeader()->setVisible(false);
    detailDcTable->horizontalHeader()->setStretchLastSection(true);

    QVBoxLayout *detailDcLayout = new QVBoxLayout;
    detailDcLayout->addWidget(detailDcTable);
    DetailDc->setLayout(detailDcLayout);

}


void UkpmWidget::setupBtrUI()
{
    tabWidgetBTR = new QTabWidget();
    QWidget *detailBTR = new QWidget();
    tabWidgetBTR->addTab(detailBTR,QString());
    tabWidgetBTR->setTabText(0,tr("Detail"));
    stackedWidget->addWidget(tabWidgetBTR);
    QStringList strList;
    strList << tr("attribute") << tr("value");
    detailBTRTable = new QTableWidget(18,2,detailBTR);
    detailBTRTable->setHorizontalHeaderLabels(strList);
    detailBTRTable->verticalHeader()->setVisible(false);
    detailBTRTable->horizontalHeader()->setStretchLastSection(true);

    detailBTRTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    detailBTRTable->setItem(0,0,new QTableWidgetItem(tr("Device")));
    detailBTRTable->setItem(1,0,new QTableWidgetItem(tr("Type")));
    detailBTRTable->setItem(2,0,new QTableWidgetItem(tr("Vendor")));
    detailBTRTable->setItem(3,0,new QTableWidgetItem(tr("Model")));
    detailBTRTable->setItem(4,0,new QTableWidgetItem(tr("PowerSupply")));
    detailBTRTable->setItem(5,0,new QTableWidgetItem(tr("Refresh")));
    detailBTRTable->setItem(6,0,new QTableWidgetItem(tr("IsPresent")));
    detailBTRTable->setItem(7,0,new QTableWidgetItem(tr("IsRechargeable")));
    detailBTRTable->setItem(8,0,new QTableWidgetItem(tr("State")));
    detailBTRTable->setItem(9,0,new QTableWidgetItem(tr("Energy")));
    detailBTRTable->setItem(10,0,new QTableWidgetItem(tr("EnergyFull")));
    detailBTRTable->setItem(11,0,new QTableWidgetItem(tr("EnergyFullDesign")));
    detailBTRTable->setItem(12,0,new QTableWidgetItem(tr("EnergyRate")));
    detailBTRTable->setItem(13,0,new QTableWidgetItem(tr("Voltage")));
    detailBTRTable->setItem(14,0,new QTableWidgetItem(tr("TimeToFull")));
    detailBTRTable->setItem(15,0,new QTableWidgetItem(tr("TimeToEmpty")));
    detailBTRTable->setItem(16,0,new QTableWidgetItem(tr("Percentage")));
    detailBTRTable->setItem(17,0,new QTableWidgetItem(tr("Capacity")));

    detailBTRTable->setItem(0,1,new QTableWidgetItem(btrDetailData.Device));
    detailBTRTable->setItem(1,1,new QTableWidgetItem(btrDetailData.Type));
    detailBTRTable->setItem(2,1,new QTableWidgetItem(btrDetailData.Vendor));
    detailBTRTable->setItem(3,1,new QTableWidgetItem(btrDetailData.Model));
    detailBTRTable->setItem(4,1,new QTableWidgetItem(btrDetailData.PowerSupply));
    detailBTRTable->setItem(5,1,new QTableWidgetItem(btrDetailData.Refresh));
    detailBTRTable->setItem(6,1,new QTableWidgetItem(btrDetailData.IsPresent));
    detailBTRTable->setItem(7,1,new QTableWidgetItem(btrDetailData.IsRechargeable));
    detailBTRTable->setItem(8,1,new QTableWidgetItem(btrDetailData.State));
    detailBTRTable->setItem(9,1,new QTableWidgetItem(btrDetailData.Energy));
    detailBTRTable->setItem(10,1,new QTableWidgetItem(btrDetailData.EnergyFull));
    detailBTRTable->setItem(11,1,new QTableWidgetItem(btrDetailData.EnergyFullDesign));
    detailBTRTable->setItem(12,1,new QTableWidgetItem(btrDetailData.EnergyRate));
    detailBTRTable->setItem(13,1,new QTableWidgetItem(btrDetailData.Voltage));
    detailBTRTable->setItem(14,1,new QTableWidgetItem(btrDetailData.TimeToFull));
    detailBTRTable->setItem(15,1,new QTableWidgetItem(btrDetailData.TimeToEmpty));
    detailBTRTable->setItem(16,1,new QTableWidgetItem(btrDetailData.Percentage));
    detailBTRTable->setItem(17,1,new QTableWidgetItem(btrDetailData.Capacity));


    QVBoxLayout *detailBTRLayout = new QVBoxLayout;
    detailBTRLayout->addWidget(detailBTRTable);
    detailBTR->setLayout(detailBTRLayout);

    setHistoryTab();
    setSumTab();

}

void UkpmWidget::initUI()
{
    QDesktopWidget *deskdop = QApplication::desktop();
    resize(deskdop->width()/2,deskdop->height()/2);
    move((deskdop->width() - this->width())/2, (deskdop->height() - this->height())/2);
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);
    setWindowTitle(tr("Power Statistics"));

    QSplitter *mainsplitter = new QSplitter(Qt::Horizontal,this);//splittering into two parts
    listWidget = new QListWidget(mainsplitter);
    stackedWidget =  new QStackedWidget(mainsplitter);

    mainsplitter->setStretchFactor(1,4);
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(mainsplitter);

    QHBoxLayout *hlayout = new QHBoxLayout;
    helpButton = new QToolButton;
    helpButton->setText(tr("help"));
    helpButton->setIcon(QIcon(":/dc.png"));
    exitButton = new QToolButton;
    helpButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    exitButton->setText(tr("exit"));
    exitButton->setIcon(QIcon(":/dc.png"));
    exitButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    hlayout->addWidget(helpButton);
    hlayout->addStretch();
    hlayout->addWidget(exitButton);

    vlayout->addLayout(hlayout);
    setLayout(vlayout);//main layout of the UI

}

void UkpmWidget::setSumTab()
{
    QWidget *tabSumBTR = new QWidget();
    tabWidgetBTR->addTab(tabSumBTR,QString());
    tabWidgetBTR->setTabText(2,tr("Statistics"));
    QLabel *graphicType = new QLabel(tr("graphic type:"),tabSumBTR);
    sumTypeCombox = new QComboBox(tabSumBTR);
    sumTypeCombox->addItems(QStringList()<<tr("charge")<<tr("charge-accurency")<<tr("discharge")<<tr("discharge-accurency"));

    sumCurveBox = new QCheckBox(tr("spline"),tabSumBTR);
    sumDataBox = new QCheckBox(tr("show datapoint"),tabSumBTR);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    QFormLayout *topLayout = new QFormLayout;
    topLayout->addRow(graphicType,sumTypeCombox);

    sumChart = new QChart;
    x = new QValueAxis;
    y = new QValueAxis;

    sumSeries = new QLineSeries();
    sumSpline = new QSplineSeries();

    sumChart->addSeries(sumSpline);
    sumChart->setAxisX(x);
    sumChart->setAxisY(y);
    sumSpline->attachAxis(x);//连接数据集与轴
    sumSpline->attachAxis(y);//连接数据集与轴

    sumSeries->setPointsVisible(true);
    sumSpline->setPointsVisible(true);

    sumChart->legend()->hide();
    sumChart->setAnimationOptions(QChart::SeriesAnimations);

    sumChartView = new QChartView(sumChart);
    sumChartView->setRenderHint(QPainter::Antialiasing);

    bottomLayout->addWidget(sumCurveBox);
    bottomLayout->addWidget(sumDataBox);
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(topLayout);
    vLayout->addWidget(sumChartView);
    vLayout->addLayout(bottomLayout);
    tabSumBTR->setLayout(vLayout);

}
void UkpmWidget::showHisDataPoint(bool flag)
{
    settings->setBool(GPM_SETTINGS_INFO_HISTORY_GRAPH_POINTS,flag);
    if(flag)
    {
        hisSeries->setPointsVisible(true);
        hisSpline->setPointsVisible(true);
    }
    else
    {
        hisSeries->setPointsVisible(false);
        hisSpline->setPointsVisible(false);
    }

}

void UkpmWidget::showSumDataPoint(bool flag)
{
    settings->setBool(GPM_SETTINGS_INFO_STATS_GRAPH_POINTS,flag);

    if(flag)
    {
        sumSeries->setPointsVisible(true);
        sumSpline->setPointsVisible(true);
    }
    else
    {
        sumSeries->setPointsVisible(false);
        sumSpline->setPointsVisible(false);
    }

}

void UkpmWidget::setHistoryTab()
{
    QWidget *tabHisBTR = new QWidget();
    tabWidgetBTR->addTab(tabHisBTR,QString());
    tabWidgetBTR->setTabText(1,tr("History"));
    QLabel *graphicType = new QLabel(tr("graphic type:"),tabHisBTR);
    graphicType->setScaledContents(true);
    QLabel *timeLabel = new QLabel(tr("time span:"),tabHisBTR);
    typeCombox = new QComboBox(tabHisBTR);
    typeCombox->addItems(QStringList()<<tr("rate")<<tr("energy")<<tr("charge-time")<<tr("discharge-time"));

    spanCombox = new QComboBox(tabHisBTR);
    spanCombox->addItems(QStringList()<<tr("ten minutes")<<tr("two hours")<<tr("six hours")<<tr("one day")<<tr("one week"));

    hisCurveBox = new QCheckBox(tr("spline"),tabHisBTR);
    hisDataBox = new QCheckBox(tr("show datapoint"),tabHisBTR);
    QHBoxLayout *topLayout = new QHBoxLayout;
    QHBoxLayout *bottomLayout = new QHBoxLayout;

    QFormLayout *hisType = new QFormLayout;
    hisType->addRow(graphicType,typeCombox);
    QFormLayout *hisSpan = new QFormLayout;
    hisSpan->addRow(timeLabel,spanCombox);
    topLayout->addLayout(hisType);
    topLayout->addLayout(hisSpan);

    bottomLayout->addWidget(hisCurveBox);
    bottomLayout->addWidget(hisDataBox);

    hisChart = new QChart;

    hisSeries = new QLineSeries();
    hisSpline = new QSplineSeries();
    hisChart->addSeries(hisSpline);

    xtime = new QCategoryAxis();//轴变量、数据系列变量，都不能声明为局部临时变量
    axisY = new QCategoryAxis();
    xtime->setTitleText(tr("elapsed time"));
    xtime->setReverse(true);
    hisChart->setAxisX(xtime);
    hisChart->setAxisY(axisY);

    hisSpline->attachAxis(xtime);//连接数据集与轴

    hisSpline->attachAxis(axisY);
    hisSeries->setPointsVisible(true);
    hisSpline->setPointsVisible(true);

    hisChart->legend()->hide();
    hisChart->setAnimationOptions(QChart::SeriesAnimations);
    hisChartView = new QChartView(hisChart);
    hisChartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(topLayout);
    vLayout->addWidget(hisChartView);
    vLayout->addLayout(bottomLayout);
    tabHisBTR->setLayout(vLayout);
}

void UkpmWidget::updateSumChart(int index)
{
    qDebug()<<"updateSumchart:"<<index;
    QString sumType;
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.UPower",batterySvr,
            "org.freedesktop.UPower.Device","GetStatistics");
    QList<QPointF> list;
    QList<QPointF> data;
    QPointF pit;
    int start_x = 0;
    int max_y = 0;
    if(index == CHARGE)
    {
        sumType = GPM_STATS_CHARGE_DATA_VALUE;
        msg << "charging";
        QDBusMessage res = QDBusConnection::systemBus().call(msg);
        if(res.type() == QDBusMessage::ReplyMessage)
        {
            printf("get %d arg from bus!\n",res.arguments().count());
            QDBusArgument dbusArg = res.arguments().at(0).value<QDBusArgument>();
            dbusArg >> list;
        }
        else {
            qDebug()<<"error of qdbus reply";

        }
        foreach(pit, list)
        {
            data.append(QPointF(start_x++,pit.x()));
            if(max_y < abs(pit.x()))
                max_y = abs(pit.x());
        }
        sumSeries->replace(data);
        sumSpline->replace(data);
        y->setTitleText(tr("adjust factor"));
        y->setRange(-max_y,max_y);
        y->setLabelFormat("%.1f");
        x->setTitleText(tr("battery power"));
        x->setRange(0,100);
        x->setLabelFormat("%d%");
        x->setTickCount(10);
        y->setTickCount(10);
    }
    else if(index == CHARGE_ACCURENCY)
    {
        sumType = GPM_STATS_CHARGE_ACCURACY_VALUE;
        msg << "charging";
        QDBusMessage res = QDBusConnection::systemBus().call(msg);
        if(res.type() == QDBusMessage::ReplyMessage)
        {
            QDBusArgument dbusArg = res.arguments().at(0).value<QDBusArgument>();
            dbusArg >> list;
        }
        else {
            qDebug()<<"error of qdbus reply";

        }
        foreach(pit, list)
        {
            data.append(QPointF(start_x++,pit.y()));
            if(max_y < (pit.y()))
                max_y = (pit.y());
        }
        sumSeries->replace(data);
        sumSpline->replace(data);
        y->setTitleText(tr("Predict Accurency"));
        y->setRange(0,max_y);
        y->setLabelFormat("%d%");
        x->setTitleText(tr("battery power"));
        x->setRange(0,100);
        x->setLabelFormat("%d%");
        x->setTickCount(10);
        y->setTickCount(10);
    }
    else if(index == DISCHARGING)
    {
        sumType = GPM_STATS_DISCHARGE_DATA_VALUE;
        msg << "discharging";
        QDBusMessage res = QDBusConnection::systemBus().call(msg);
        if(res.type() == QDBusMessage::ReplyMessage)
        {
            QDBusArgument dbusArg = res.arguments().at(0).value<QDBusArgument>();
            dbusArg >> list;
        }
        else {
            qDebug()<<"error of qdbus reply";

        }
        foreach(pit, list)
        {
            data.append(QPointF(start_x++,pit.x()));
            if(max_y < abs(pit.x()))
                max_y = abs(pit.x());
        }
        sumSeries->replace(data);
        sumSpline->replace(data);
        y->setTitleText(tr("adjust factor"));
        y->setRange(-max_y,max_y);
        y->setLabelFormat("%.1f");
        x->setTitleText(tr("battery power"));
        x->setRange(0,100);
        x->setLabelFormat("%d%");
        x->setTickCount(10);
        y->setTickCount(10);
    }
    else if(index == DISCHARGING_ACCURENCY)
    {
        sumType = GPM_STATS_DISCHARGE_ACCURACY_VALUE;
        msg << "discharging";
        QDBusMessage res = QDBusConnection::systemBus().call(msg);
        if(res.type() == QDBusMessage::ReplyMessage)
        {
            QDBusArgument dbusArg = res.arguments().at(0).value<QDBusArgument>();
            dbusArg >> list;
        }
        else {
            qDebug()<<"error of qdbus reply";

        }
        foreach(pit, list)
        {
            data.append(QPointF(start_x++,pit.y()));
            if(max_y < (pit.y()))
                max_y = (pit.y());
        }
        sumSeries->replace(data);
        sumSpline->replace(data);
        y->setTitleText(tr("Predict Accurency"));
        y->setRange(0,max_y);
        y->setLabelFormat("%d%");
        x->setTitleText(tr("battery power"));
        x->setRange(0,100);
        x->setLabelFormat("%d%");
        x->setTickCount(10);
        y->setTickCount(10);
    }
    settings->setString(GPM_SETTINGS_INFO_STATS_TYPE,sumType);

}
void UkpmWidget::onExitButtonClicked(bool)
{
    close();
}

void UkpmWidget::onHelpButtonClicked(bool)
{

}

void UkpmWidget::callFinishedSlot(QDBusPendingCallWatcher* call)
{
    QDBusPendingReply<QList<QDBusVariant>> reply = *call;
    if(!reply.isError()){
        QList<QDBusVariant> listQDBus;
        listQDBus= reply.argumentAt<0>();
        qDebug()<<"dsfdsfs";
    }
    call->deleteLater();
}

void UkpmWidget::updateHisType(int index)
{
    QString hisType;
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.UPower",batterySvr,
            "org.freedesktop.UPower.Device","GetHistory");
    QList<QPointF> list;
    QList<StructUdu> listQDBus;
    QDBusVariant item;
    QVariant variant;
    uint size;
    QPointF temp;
    QDBusArgument argument;
    mHISTYPE = (HISTYPE)index;
    struct timeval tv;
    gettimeofday(&tv,NULL);
//    uint32_t prd = (uint32_t)tv.tv_sec;
    int max_y = 0;
//    int min_y = 0;
    if(index == RATE)
    {
        hisType = GPM_HISTORY_RATE_VALUE;
        msg << "rate" << timeSpan << resolution;
        QDBusMessage res = QDBusConnection::systemBus().call(msg);
        if(res.type() == QDBusMessage::ReplyMessage)
        {
            variant = res.arguments().at(0);
            argument = variant.value<QDBusArgument>();
            argument >> listQDBus;
            size = listQDBus.size();
            qDebug()<<size;
            for(uint i = 0; i< size; i++)
            {
                if(listQDBus[i].state == 0)
                    continue;
                temp.setX(listQDBus[0].time - listQDBus[i].time);
                temp.setY(listQDBus[i].value);
                list.append(temp);
            }
        }
        else {
            qDebug()<<"error of qdbus reply";

        }

        qDebug()<< list.size();
        hisSeries->replace(list);
        hisSpline->replace(list);
        axisY->setTitleText(tr("Rate"));

        QStringList labels = axisY->categoriesLabels();
        foreach (QString str, labels) {
            axisY->remove(str);
        }
        axisY->setMin(0.0);
        axisY->setMax(10.0);
        axisY->setStartValue(0.0);
        for(int i = 0; i < 11; i++)
        {
            QString str;
            str.sprintf("%.1fW",i*1.0);
            axisY->append(str,i*1.0);
        }
        axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    }
    else if(index == VOLUME)
    {
        hisType = GPM_HISTORY_CHARGE_VALUE;
        msg << "charge" << timeSpan << resolution;
        QDBusMessage res = QDBusConnection::systemBus().call(msg);
        if(res.type() == QDBusMessage::ReplyMessage)
        {
            variant = res.arguments().at(0);
            argument = variant.value<QDBusArgument>();
            argument >> listQDBus;
            size = listQDBus.size();
            qDebug()<<size;
            for(uint i = 0; i< size; i++)
            {
                if(listQDBus[i].state == 0)
                    continue;
                temp.setX(listQDBus[0].time - listQDBus[i].time);
                temp.setY(listQDBus[i].value);
                list.append(temp);
            }
        }
        else {
            qDebug()<<"error of qdbus reply";

        }

        qDebug()<< list.size();
        hisSeries->replace(list);
        hisSpline->replace(list);
        axisY->setTitleText(tr("Charge"));

        QStringList labels = axisY->categoriesLabels();
        foreach (QString str, labels) {
            axisY->remove(str);
        }
        axisY->setMin(0);
        axisY->setMax(100);
        axisY->setStartValue(0);
        for(int i = 0; i < 11; i++)
        {
            QString str;
            str.sprintf("%d%%",i*10);
            axisY->append(str,i*10);
        }
        axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    }
    else if(index == CHARGE_DURATION)
    {
        hisType = GPM_HISTORY_TIME_FULL_VALUE;
        msg << "time-full" << timeSpan << resolution;
        QDBusMessage res = QDBusConnection::systemBus().call(msg);
        if(res.type() == QDBusMessage::ReplyMessage)
        {
            variant = res.arguments().at(0);
            argument = variant.value<QDBusArgument>();
            argument >> listQDBus;
            size = listQDBus.size();
            qDebug()<<size;
            for(uint i = 0; i< size; i++)
            {
                if(listQDBus[i].state == 0)
                    continue;
                temp.setX(listQDBus[0].time - listQDBus[i].time);
                temp.setY(listQDBus[i].value);
                if(max_y < temp.y())   //||(min_y>temp.x())
                    max_y = temp.y();

                list.append(temp);
            }
        }
        else {
            qDebug()<<"error of qdbus reply";
        }
        qDebug()<< list.size();
        hisSeries->replace(list);
        hisSpline->replace(list);
        axisY->setTitleText(tr("Predict Time"));
        QStringList labels = axisY->categoriesLabels();
        foreach (QString str, labels) {
            axisY->remove(str);
        }
        axisY->setMin(0);
        axisY->setMax(max_y);
        axisY->setStartValue(0);
        for(int i = 0; i < 11; i++)
        {
            axisY->append(getWidgetAxis(i*max_y/10),i*max_y/10);
        }
        axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    }
    else if(index == DISCHARGING_DURATION)
    {
        hisType = GPM_HISTORY_TIME_EMPTY_VALUE;
        msg << "time-empty" << timeSpan << resolution;
        QDBusMessage res = QDBusConnection::systemBus().call(msg);
        if(res.type() == QDBusMessage::ReplyMessage)
        {
            variant = res.arguments().at(0);
            argument = variant.value<QDBusArgument>();
            argument >> listQDBus;
            size = listQDBus.size();
            qDebug()<<size;
            for(uint i = 0; i< size; i++)
            {
                if(listQDBus[i].state == 0)
                    continue;
                temp.setX(listQDBus[0].time - listQDBus[i].time);
                temp.setY(listQDBus[i].value);
                if(max_y < temp.y())   //||(min_y>temp.x())
                    max_y = temp.y();

                list.append(temp);
            }
        }
        else {
            qDebug()<<"error of qdbus reply";
        }

        qDebug()<< list.size();
        hisSeries->replace(list);
        hisSpline->replace(list);
        axisY->setTitleText(tr("Predict Time"));

        QStringList labels = axisY->categoriesLabels();
        foreach (QString str, labels) {
            axisY->remove(str);
        }
        axisY->setMin(0);
        axisY->setMax(max_y);
        axisY->setStartValue(0);
        for(int i = 0; i < 11; i++)
        {
            axisY->append(getWidgetAxis(i*max_y/10),i*max_y/10);
        }
        axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    }
    settings->setString(GPM_SETTINGS_INFO_HISTORY_TYPE,hisType);

}

QString UkpmWidget::getWidgetAxis(uint value)
{
    QString text;
    uint times = value;
    uint minutes = times/60;
    uint seconds = times - (minutes *60);
    uint hours = minutes /60;
    uint days = hours / 24;
    minutes = minutes - (hours*60);
    hours = hours - (days*24);
    if(days >0)
    {
        if(hours ==0)
            text.sprintf("%dd",days);
        else
            text.sprintf("%dd%dh",days,hours);
    }
    else if(hours > 0)
    {
        if(minutes ==0)
            text.sprintf("%dh",hours);
        else
            text.sprintf("%dh%dm",hours,minutes);
    }
    else if(minutes > 0)
    {
        if(seconds ==0)
            text.sprintf("%dm",minutes);
        else
            text.sprintf("%dm%ds",minutes,seconds);
    }
    else
    {
        text.sprintf("%ds",seconds);
    }
    return text;

}

void UkpmWidget::updateHisChart(int index)
{
    if(index == TENM)
    {
        timeSpan = 600;
    }
    else if(index == TWOH)
    {
        timeSpan = 2*60*60;
    }
    else if(index == SIXH)
    {
        timeSpan = 6*60*60;
    }
    else if(index == ONED)
    {
        timeSpan = 24*60*60;
    }
    else if(index == ONEW)
    {
        timeSpan = 7*24*60*60;
    }

    QStringList labels = xtime->categoriesLabels();
    foreach (QString str, labels) {
        xtime->remove(str);
    }
    xtime->setMin(0);
    xtime->setMax(timeSpan);
    xtime->setStartValue(0);
    for(int i = 0; i < 11; i++)
    {
        xtime->append(getWidgetAxis(i*timeSpan/10),i*timeSpan/10);
    }
    xtime->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    settings->setInt(GPM_SETTINGS_INFO_HISTORY_TIME,timeSpan);
    updateHisType(mHISTYPE);

}
void UkpmWidget::sortDcTable(int id)
{
    detailDcTable->sortByColumn(id,Qt::AscendingOrder);
}

void UkpmWidget::sortBtrTable(int id)
{
    detailBTRTable->sortByColumn(id,Qt::AscendingOrder);
}

void UkpmWidget::onListChanged(int row)
{
    stackedWidget->setCurrentIndex(row);
    if(1 == row)
    {
        getDcDetail();
        detailDcTable->item(0,1)->setText(dcDetailData.Device);
        detailDcTable->item(1,1)->setText(dcDetailData.Type);
        detailDcTable->item(2,1)->setText(dcDetailData.PowerSupply);
        detailDcTable->item(3,1)->setText(dcDetailData.Online);
    }
}

void UkpmWidget::connectSlots()
{
    if(devices.size() == 0)
	return;

    QDBusConnection::systemBus().connect(DBUS_SERVICE,batterySvr,DBUS_INTERFACE,
                                         QString("PropertiesChanged"),this,SLOT(btrPropertiesChanged(QDBusMessage)));
    QDBusConnection::systemBus().connect(DBUS_SERVICE,acSvr,DBUS_INTERFACE,
                                         QString("PropertiesChanged"),this,SLOT(acPropertiesChanged(QDBusMessage)));

    QDBusConnection::systemBus().connect(DBUS_SERVICE,DBUS_OBJECT,DBUS_SERVICE,
                                         QString("device-added"),this,SLOT(deviceAdded(QDBusMessage)));
    QDBusConnection::systemBus().connect(DBUS_SERVICE,DBUS_OBJECT,DBUS_SERVICE,
                                         QString("device-removed"),this,SLOT(deviceRemoved(QDBusMessage)));

    connect(listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(onListChanged(int)));
    connect(typeCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateHisType(int)));
    connect(spanCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateHisChart(int)));
    connect(sumTypeCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateSumChart(int)));
    connect(exitButton,SIGNAL(clicked(bool)),this,SLOT(onExitButtonClicked(bool)));
    connect(sumDataBox,SIGNAL(clicked(bool)),this,SLOT(showSumDataPoint(bool)));
    connect(hisDataBox,SIGNAL(clicked(bool)),this,SLOT(showHisDataPoint(bool)));
    connect(hisCurveBox,SIGNAL(clicked(bool)),this,SLOT(drawHisSpineline(bool)));
    connect(sumCurveBox,SIGNAL(clicked(bool)),this,SLOT(drawSumSpineline(bool)));
    connect(helpButton,SIGNAL(clicked(bool)),this,SLOT(helpFormat()));
    connect(tabWidgetBTR,SIGNAL(currentChanged(int)),this,SLOT(onBtrPageChanged(int)));

    bool checked;
    uint page = settings->getInt(GPM_SETTINGS_INFO_PAGE_NUMBER);
    tabWidgetBTR->setCurrentIndex(page);

    QString history_type = settings->getString(GPM_SETTINGS_INFO_HISTORY_TYPE);
    int history_time = settings->getInt(GPM_SETTINGS_INFO_HISTORY_TIME);
    if (history_type == NULL)
        history_type = GPM_HISTORY_CHARGE_VALUE;
    if (history_time == 0)
        history_time = GPM_HISTORY_HOUR_VALUE;

    if (QString::compare (history_type, GPM_HISTORY_RATE_VALUE) == 0)
        typeCombox->setCurrentIndex(0);
    else
        typeCombox->setCurrentIndex(1);

    if (history_time == GPM_HISTORY_MINUTE_VALUE)
        spanCombox->setCurrentIndex(0);
    else if (history_time == GPM_HISTORY_HOUR_VALUE)
        spanCombox->setCurrentIndex(1);
    else
        spanCombox->setCurrentIndex(2);


    QString stats_type = settings->getString(GPM_SETTINGS_INFO_STATS_TYPE);
    if(stats_type == NULL)
        stats_type = GPM_STATS_CHARGE_DATA_VALUE;
    qDebug()<< stats_type;
    if (QString::compare( stats_type, GPM_STATS_CHARGE_DATA_VALUE) == 0)
        sumTypeCombox->setCurrentIndex(0);
    else if (QString::compare(stats_type, GPM_STATS_CHARGE_ACCURACY_VALUE) == 0)
        sumTypeCombox->setCurrentIndex(1);
    else if (QString::compare (stats_type, GPM_STATS_DISCHARGE_DATA_VALUE) == 0)
        sumTypeCombox->setCurrentIndex(2);
    else
        sumTypeCombox->setCurrentIndex(3);

    Q_EMIT sumTypeCombox->currentIndexChanged(sumTypeCombox->currentIndex());
    Q_EMIT spanCombox->currentIndexChanged(spanCombox->currentIndex());
    checked = settings->getBool(GPM_SETTINGS_INFO_HISTORY_GRAPH_SMOOTH);
    hisCurveBox->setChecked(checked);
    Q_EMIT hisCurveBox->clicked(checked);
    checked = settings->getBool(GPM_SETTINGS_INFO_HISTORY_GRAPH_POINTS);
    hisDataBox->setChecked(checked);
    Q_EMIT hisDataBox->clicked(checked);
    checked = settings->getBool(GPM_SETTINGS_INFO_STATS_GRAPH_SMOOTH);
    sumCurveBox->setChecked(checked);
    Q_EMIT sumCurveBox->clicked(checked);
    checked = settings->getBool(GPM_SETTINGS_INFO_STATS_GRAPH_POINTS);
    sumDataBox->setChecked(checked);
    Q_EMIT sumDataBox->clicked(checked);
    Q_EMIT tabWidgetBTR->currentChanged(page);
    listWidget->setCurrentRow(parseArguments());
}

void UkpmWidget::onBtrPageChanged(int index)
{
    settings->setInt(GPM_SETTINGS_INFO_PAGE_NUMBER,index);
    qDebug()<<"btr page number is:"<< index;
    if(0 == index)
    {
    //jiangh
    getBtrDetail();
//    detailBTRTable->itemAt(0,1)->setData(Qt::EditRole,btrDetailData.Device);
//    detailBTRTable->itemAt(1,1)->setData(Qt::EditRole,btrDetailData.Type);
//    detailBTRTable->itemAt(2,1)->setData(Qt::EditRole,btrDetailData.Vendor);
//    detailBTRTable->itemAt(3,1)->setData(Qt::EditRole,btrDetailData.Model);
//    detailBTRTable->itemAt(4,1)->setData(Qt::EditRole,btrDetailData.PowerSupply);
//    detailBTRTable->itemAt(5,1)->setData(Qt::EditRole,btrDetailData.Refresh);
//    detailBTRTable->itemAt(6,1)->setData(Qt::EditRole,btrDetailData.IsPresent);
//    detailBTRTable->itemAt(7,1)->setData(Qt::EditRole,btrDetailData.IsRechargeable);
//    detailBTRTable->itemAt(8,1)->setData(Qt::EditRole,btrDetailData.State);
//    detailBTRTable->itemAt(9,1)->setData(Qt::EditRole,btrDetailData.Energy);
//    detailBTRTable->itemAt(10,1)->setData(Qt::EditRole,btrDetailData.EnergyFull);
//    detailBTRTable->itemAt(11,1)->setData(Qt::EditRole,btrDetailData.EnergyFullDesign);
//    detailBTRTable->itemAt(12,1)->setData(Qt::EditRole,btrDetailData.EnergyRate);
//    detailBTRTable->itemAt(13,1)->setData(Qt::EditRole,btrDetailData.Voltage);
//    detailBTRTable->itemAt(14,1)->setData(Qt::EditRole,btrDetailData.TimeToFull);
//    detailBTRTable->itemAt(15,1)->setData(Qt::EditRole,btrDetailData.TimeToEmpty);
//    detailBTRTable->itemAt(16,1)->setData(Qt::EditRole,btrDetailData.Percentage);
//    detailBTRTable->itemAt(17,1)->setData(Qt::EditRole,btrDetailData.Capacity);
        detailBTRTable->item(1,1)->setText(btrDetailData.Type);
        detailBTRTable->item(2,1)->setText(btrDetailData.Vendor);
        detailBTRTable->item(3,1)->setText(btrDetailData.Model);
        detailBTRTable->item(4,1)->setText(btrDetailData.PowerSupply);
        detailBTRTable->item(5,1)->setText(btrDetailData.Refresh);
        detailBTRTable->item(6,1)->setText(btrDetailData.IsPresent);
        detailBTRTable->item(7,1)->setText(btrDetailData.IsRechargeable);
        detailBTRTable->item(8,1)->setText(btrDetailData.State);
        detailBTRTable->item(9,1)->setText(btrDetailData.Energy);
        detailBTRTable->item(10,1)->setText(btrDetailData.EnergyFull);
        detailBTRTable->item(11,1)->setText(btrDetailData.EnergyFullDesign);
        detailBTRTable->item(12,1)->setText(btrDetailData.EnergyRate);
        detailBTRTable->item(13,1)->setText(btrDetailData.Voltage);
        detailBTRTable->item(14,1)->setText(btrDetailData.TimeToFull);
        detailBTRTable->item(15,1)->setText(btrDetailData.TimeToEmpty);
        detailBTRTable->item(16,1)->setText(btrDetailData.Percentage);
        detailBTRTable->item(17,1)->setText(btrDetailData.Capacity);
    }
   if(1 == index)
    {
        Q_EMIT spanCombox->currentIndexChanged(spanCombox->currentIndex());
    }
   if(2 == index)
    {
        Q_EMIT sumTypeCombox->currentIndexChanged(sumTypeCombox->currentIndex());
    }
}

void UkpmWidget::drawHisSpineline(bool flag)
{
    settings->setBool(GPM_SETTINGS_INFO_HISTORY_GRAPH_SMOOTH,flag);
    if(flag)
    {
        spineLineHis = true;
        hisChart->removeSeries(hisSeries);
        hisChart->addSeries(hisSpline);
        hisSpline->attachAxis(xtime);//连接数据集与
        hisSpline->attachAxis(axisY);//连接数据集与
    }
    else
    {
        spineLineHis = false;
        hisChart->removeSeries(hisSpline);
        hisChart->addSeries(hisSeries);
        hisSeries->attachAxis(xtime);//连接数据集与
        hisSeries->attachAxis(axisY);//连接数据集与
    }
}

void UkpmWidget::drawSumSpineline(bool flag)
{
    settings->setBool(GPM_SETTINGS_INFO_STATS_GRAPH_SMOOTH,flag);
    if(flag)
    {
        spineLineSum = true;
        sumChart->removeSeries(sumSeries);
        sumChart->addSeries(sumSpline);
        sumSpline->attachAxis(x);
        sumSpline->attachAxis(y);
    }
    else
    {
        spineLineSum = false;
        sumChart->removeSeries(sumSpline);
        sumChart->addSeries(sumSeries);
        sumSeries->attachAxis(x);
        sumSeries->attachAxis(y);
    }
}

void UkpmWidget::setupBtrDetail()
{
    tabWidgetBTR = new QTabWidget();
    QWidget *detailBTR = new QWidget();
    tabWidgetBTR->addTab(detailBTR,QString());
    tabWidgetBTR->setTabText(0,tr("Detail"));
    stackedWidget->addWidget(tabWidgetBTR);
    QStringList strList;
    strList << tr("attribute") << tr("value");
    detailBTRTable = new QTableWidget(18,2,detailBTR);
    detailBTRTable->setHorizontalHeaderLabels(strList);
    detailBTRTable->verticalHeader()->setVisible(false);
    detailBTRTable->horizontalHeader()->setStretchLastSection(true);

    detailBTRTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    detailBTRTable->setItem(0,0,new QTableWidgetItem(tr("Device")));
    detailBTRTable->setItem(1,0,new QTableWidgetItem(tr("Type")));
    detailBTRTable->setItem(2,0,new QTableWidgetItem(tr("Vendor")));
    detailBTRTable->setItem(3,0,new QTableWidgetItem(tr("Model")));
    detailBTRTable->setItem(4,0,new QTableWidgetItem(tr("PowerSupply")));
    detailBTRTable->setItem(5,0,new QTableWidgetItem(tr("Refresh")));
    detailBTRTable->setItem(6,0,new QTableWidgetItem(tr("IsPresent")));
    detailBTRTable->setItem(7,0,new QTableWidgetItem(tr("IsRechargeable")));
    detailBTRTable->setItem(8,0,new QTableWidgetItem(tr("State")));
    detailBTRTable->setItem(9,0,new QTableWidgetItem(tr("Energy")));
    detailBTRTable->setItem(10,0,new QTableWidgetItem(tr("EnergyFull")));
    detailBTRTable->setItem(11,0,new QTableWidgetItem(tr("EnergyFullDesign")));
    detailBTRTable->setItem(12,0,new QTableWidgetItem(tr("EnergyRate")));
    detailBTRTable->setItem(13,0,new QTableWidgetItem(tr("Voltage")));
    detailBTRTable->setItem(14,0,new QTableWidgetItem(tr("TimeToFull")));
    detailBTRTable->setItem(15,0,new QTableWidgetItem(tr("TimeToEmpty")));
    detailBTRTable->setItem(16,0,new QTableWidgetItem(tr("Percentage")));
    detailBTRTable->setItem(17,0,new QTableWidgetItem(tr("Capacity")));

    QVBoxLayout *detailBTRLayout = new QVBoxLayout;
    detailBTRLayout->addWidget(detailBTRTable);
    detailBTR->setLayout(detailBTRLayout);
}

void UkpmWidget::getDcDetail()
{
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.UPower",acSvr,
            "org.freedesktop.DBus.Properties","GetAll");
    msg << "org.freedesktop.UPower.Device";
    QDBusMessage res = QDBusConnection::systemBus().call(msg);

    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        QMap<QString,QVariant> map;
        dbusArg >> map;

        dcDetailData.Type = (map.value(QString("Type")).toInt()==2) ? tr("Notebook battery") : tr("ac-adapter");
        dcDetailData.Online = boolToString(map.value(QString("Online")).toBool());
        dcDetailData.PowerSupply = boolToString(map.value(QString("PowerSupply")).toBool());
        dcDetailData.Device = acSvr.section('/',-1);
    }
    else {
        qDebug()<<"No response!";
    }

}

void UkpmWidget::getBtrDetail()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(DBUS_SERVICE,batterySvr,
            DBUS_INTERFACE,"GetAll");
    msg << DBUS_INTERFACE_PARAM;
    QDBusMessage res = QDBusConnection::systemBus().call(msg);

    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        QMap<QString,QVariant> map;
        dbusArg >> map;

        btrDetailData.Type = up_device_kind_to_string ((UpDeviceKind)map.value(QString("Type")).toInt());
        btrDetailData.Model = map.value(QString("Model")).toString();
        btrDetailData.Device = map.value(QString("NativePath")).toString();
        btrDetailData.Vendor = map.value(QString("Vendor")).toString();
        btrDetailData.Capacity = QString::number(map.value(QString("Capacity")).toDouble(), 'f', 1) + "%";
        btrDetailData.Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
        btrDetailData.EnergyEmpty= QString::number(map.value(QString("EnergyEmpty")).toDouble(), 'f', 1)+ " Wh";
        btrDetailData.EnergyFull = QString::number(map.value(QString("EnergyFull")).toDouble(), 'f', 1)+ " Wh";
        btrDetailData.EnergyFullDesign = QString::number(map.value(QString("EnergyFullDesign")).toDouble(), 'f', 1) + " Wh";
        btrDetailData.EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
        btrDetailData.IsPresent = boolToString(map.value(QString("IsPresent")).toBool());
        btrDetailData.IsRechargeable = boolToString(map.value(QString("IsRechargeable")).toBool());
        btrDetailData.PowerSupply = boolToString(map.value(QString("PowerSupply")).toBool());
        btrDetailData.Percentage = QString::number(map.value(QString("Percentage")).toDouble(), 'f', 1)+"%";
        btrDetailData.Online = boolToString(map.value(QString("Online")).toBool());
        struct timeval tv;
        uint tim;
        gettimeofday(&tv,NULL);
        tim = tv.tv_sec - map.value(QString("UpdateTime")).toLongLong();
        btrDetailData.Refresh = getSufix(tim,'s');

        flag = map.value(QString("State")).toLongLong();
        switch (flag) {
        case 1:
            dcDetailData.Online = tr("Yes");
            btrDetailData.State = tr("Charging");

            break;
        case 2:
            dcDetailData.Online = tr("No");
            btrDetailData.State = tr("Discharging");

            break;
        case 3:
            btrDetailData.State = tr("Empty");
            break;
        case 4:
            btrDetailData.State = tr("Charged");
            break;
        default:
            break;
        }

        calcTime(btrDetailData.TimeToEmpty, map.value(QString("TimeToEmpty")).toLongLong());
        calcTime(btrDetailData.TimeToFull, map.value(QString("TimeToFull")).toLongLong());
        btrDetailData.Voltage = QString::number(map.value(QString("Voltage")).toDouble(), 'f', 1) + " V";
    }
    else {
        qDebug()<<"No response!";
    }
}

void UkpmWidget::getAll(BTRDetail *dc)
{
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.UPower",batterySvr,
            "org.freedesktop.DBus.Properties","GetAll");
    msg << "org.freedesktop.UPower.Device";
    QDBusMessage res = QDBusConnection::systemBus().call(msg);

    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        QMap<QString,QVariant> map;
        dbusArg >> map;

//        long time = map.value(QString("UpdateTime")).toLongLong();

        dc->Type = (map.value(QString("Type")).toInt()==2) ? QString("Notebook battery") : QString("other");
        dc->Model = map.value(QString("Model")).toString();
        dc->Device = QString("battery-") + map.value(QString("NativePath")).toString();
        dc->Vendor = map.value(QString("Vendor")).toString();
        dc->Capacity = QString::number(map.value(QString("Capacity")).toDouble(), 'f', 1) + "%";
        dc->Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
        dc->EnergyEmpty= QString::number(map.value(QString("EnergyEmpty")).toDouble(), 'f', 1)+ " Wh";
        dc->EnergyFull = QString::number(map.value(QString("EnergyFull")).toDouble(), 'f', 1)+ " Wh";
        dc->EnergyFullDesign = QString::number(map.value(QString("EnergyFullDesign")).toDouble(), 'f', 1) + " Wh";
        dc->EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
        dc->IsPresent = (map.value(QString("IsPresent")).toBool()) ? QString("Yes") : QString("No");
        dc->IsRechargeable = (map.value(QString("IsRechargeable")).toBool()) ? QString("Yes") : QString("No");
        dc->Percentage = QString::number(map.value(QString("Percentage")).toDouble())+"%";
        dc->State = (map.value(QString("State")).toLongLong())==2?QString("Charging") : QString("Discharging");
        dc->TimeToEmpty= QString::number(map.value(QString("TimeToEmpty")).toLongLong() / 3600.0, 'f', 1) + " h";
        dc->TimeToFull= QString::number(map.value(QString("TimeToFull")).toLongLong() / 3600.0, 'f', 1) + " h";
        dc->Voltage = QString::number(map.value(QString("Voltage")).toLongLong()) + " V";

    }
    else {
        qDebug()<<"No response!";
    }

}
QList<QPointF> UkpmWidget::setdata() //设置图表数据的函数接口
{
    QList<QPointF> datalist;
    for (int i = 0; i < 100; i++)
        datalist.append(QPointF(i, qrand()%10));
    return datalist;
}

void UkpmWidget::control_center_power()
{
    QProcess *cmd = new QProcess(this);
    cmd->start("kylin-control-center -p");
}

void UkpmWidget::helpFormat()
{
    QProcess *cmd = new QProcess(this);
    cmd->start("yelp");
}

void UkpmWidget::btrPropertiesChanged(QDBusMessage  msg)
{
    const QDBusArgument &arg = msg.arguments().at(1).value<QDBusArgument>();
    QMap<QString,QVariant> map;
    arg >> map;
    putAttributes(map);
}

void UkpmWidget::acPropertiesChanged(QDBusMessage  msg)
{
    const QDBusArgument &arg = msg.arguments().at(1).value<QDBusArgument>();
    QMap<QString,QVariant> map;
    arg >> map;
    if(map.contains("Online"))
    {
        iconflag=map.value(QString("Online")).toBool();
        dcDetailData.Online = boolToString(iconflag);
        detailDcTable->item(3,1)->setText(dcDetailData.Online);
    }
}


void UkpmWidget::deviceAdded(QDBusMessage  msg)
{
    QListWidgetItem *item;
    QDBusObjectPath objectPath;
    QString kind,vendor,model,label;
    int kindEnum = 0;
    const QDBusArgument &arg = msg.arguments().at(0).value<QDBusArgument>();
    arg >> objectPath;

    QDBusMessage msgTmp = QDBusMessage::createMethodCall(DBUS_SERVICE,objectPath.path(),
            DBUS_INTERFACE,"GetAll");
    msgTmp << DBUS_INTERFACE_PARAM;
    QDBusMessage res = QDBusConnection::systemBus().call(msgTmp);
    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        QMap<QString,QVariant> map;
        dbusArg >> map;
        kind = map.value(QString("kind")).toString();
        if(kind.length() ==0)
            kind = map.value(QString("Type")).toString();
        kindEnum = kind.toInt();
        QString icon = up_device_kind_to_string((UpDeviceKind)kindEnum);
        vendor = map.value(QString("Vendor")).toString();
        model = map.value(QString("Model")).toString();
        if(vendor.length() != 0 && model.length() != 0)
            label = vendor + " " + model;
        else
            label =device_kind_to_localised_text((UpDeviceKind)kindEnum,1);

        if(kindEnum == UP_DEVICE_KIND_LINE_POWER || kindEnum == UP_DEVICE_KIND_BATTERY || kindEnum == UP_DEVICE_KIND_COMPUTER)
        {
            item = new QListWidgetItem(QIcon(":/"+icon),label);
            listWidget->addItem(item);
            listItem.insert(objectPath,item);
        }

        DEV dev;
        dev.Type = up_device_kind_to_string ((UpDeviceKind)map.value(QString("Type")).toInt());
        dev.Model = map.value(QString("Model")).toString();
        dev.Device = map.value(QString("NativePath")).toString();
        dev.Vendor = map.value(QString("Vendor")).toString();
        dev.Capacity = QString::number(map.value(QString("Capacity")).toDouble(), 'f', 1) + "%";
        dev.Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
        dev.EnergyEmpty= QString::number(map.value(QString("EnergyEmpty")).toDouble(), 'f', 1)+ " Wh";
        dev.EnergyFull = QString::number(map.value(QString("EnergyFull")).toDouble(), 'f', 1)+ " Wh";
        dev.EnergyFullDesign = QString::number(map.value(QString("EnergyFullDesign")).toDouble(), 'f', 1) + " Wh";
        dev.EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
        dev.Percentage = QString::number(map.value(QString("Percentage")).toDouble(), 'f', 1)+"%";
        dev.Online = boolToString(map.value(QString("Online")).toBool());
        dev.IsPresent = boolToString(map.value(QString("IsPresent")).toBool());
        dev.IsRechargeable = boolToString(map.value(QString("IsRechargeable")).toBool());
        dev.PowerSupply = boolToString(map.value(QString("PowerSupply")).toBool());

        flag = map.value(QString("State")).toLongLong();
        switch (flag) {
        case 1:
            dev.State = tr("Charging");
            break;
        case 2:
            dev.State = tr("Discharging");
            break;
        case 3:
            dev.State = tr("Empty");
            break;
        case 4:
            dev.State = tr("Charged");
            break;
        default:
            break;
        }
        calcTime(dev.TimeToEmpty, map.value(QString("TimeToEmpty")).toLongLong());
        calcTime(dev.TimeToFull, map.value(QString("TimeToFull")).toLongLong());
        dev.Voltage = QString::number(map.value(QString("Voltage")).toDouble(), 'f', 1) + " V";
        devices.push_back(dev);
        addNewUI(objectPath);
    }
}

void UkpmWidget::deviceRemoved(QDBusMessage  msg)
{
    const QDBusArgument &arg = msg.arguments().at(0).value<QDBusArgument>();
    QDBusObjectPath objectPath;
    arg >> objectPath;
    QMap<QDBusObjectPath,QListWidgetItem*>::iterator iter = listItem.find(objectPath);
    if(iter!= listItem.end())
    {
        listWidget->removeItemWidget(iter.value());
        listItem.erase(iter);
        delete iter.value();
    }
    QMap<QDBusObjectPath,QTabWidget*>::iterator iterWidget = widgetItem.find(objectPath);
    if(iterWidget!= widgetItem.end())
    {
        stackedWidget->removeWidget(iterWidget.value());
        widgetItem.erase(iterWidget);
        delete iterWidget.value();
    }
}


