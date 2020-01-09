#include "mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QProcess>
#include "ui_mainwindow.h"

#define POWER_SCHEMA "org.ukui.power-manager"
#define POWER_SCHEMA_KEY "power-manager"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ed = EngineDevice::getInstance();

    ui->setupUi(this);
     initData();
//    setting = new QGSettings(POWER_SCHEMA);
//    connect(setting,SIGNAL(changed()))
//    connect(setting,SIGNAL(changed(const QString &)),this,SLOT(iconThemeChanged()));

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/22x22/status/gpm-battery-000-charging.png"));
    trayIcon->show();
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(onActivatedIcon(QSystemTrayIcon::ActivationReason)));
    connect(ed,SIGNAL(icon_changed(QString)),this,SLOT(on_icon_changed(QString)));
    connect(ed,SIGNAL(engine_signal_summary_change(QString)),this,SLOT(on_sum_changed(QString)));

    setObjectName("MainWindow");
    initUi();
    disp_control = true;
    menu = new QMenu(this);
    set_preference  = new QAction(menu);
    show_percentage = new QAction(menu);
    set_preference->setIcon(QIcon(":/22x22/status/gpm-ac-adapter.png"));
    set_preference->setText("preference");
    show_percentage->setIcon(QIcon(":/22x22/status/gpm-ac-adapter.png"));
    show_percentage->setText("percentage");
    connect(set_preference,&QAction::triggered,this,&MainWindow::set_preference_func);
    connect(show_percentage,&QAction::triggered,this,&MainWindow::show_percentage_func);
    menu->addAction(set_preference);
    menu->addAction(show_percentage);

    trayIcon->setContextMenu(menu);
}

void MainWindow::on_sum_changed(QString str)
{
    trayIcon->setToolTip(str);
    qDebug()<<str;
}

void MainWindow::on_icon_changed(QString str)
{
    str = ":/22x22/status/"+str+".png";
    QIcon icon = QIcon(str);
    trayIcon->setIcon(icon);;
//    qDebug()<<str;
}

void MainWindow::set_preference_func()
{
    QProcess *cmd = new QProcess(this);
    cmd->start("yelp");
}

void MainWindow::show_percentage_func()
{
    QProcess *cmd = new QProcess(this);
    cmd->start("yelp");
}

void MainWindow::onActivatedIcon(QSystemTrayIcon::ActivationReason reason)
{
    get_power_list();
    QRect rect;
    switch (reason) {
    case QSystemTrayIcon::Trigger:{

            QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
            QRect screenGeometry = qApp->primaryScreen()->geometry();

            QDesktopWidget* pDesktopWidget = QApplication::desktop();
        //    int nScreenCount = QApplication::desktop()->screenCount();
        //    QRect deskRect = pDesktopWidget->availableGeometry();//可用区域
            QRect screenRect = pDesktopWidget->screenGeometry();//屏幕区域

        //    qDebug()<<"screenRect.x(): "<<screenRect.x()<<"   screenRect.height(): "<<screenRect.height();
        //    qDebug()<<"availableGeometry.y(): "<<availableGeometry.y()<<"   availableGeometry.height(): "<<availableGeometry.height();
            if (screenRect.height() != availableGeometry.height()) {
                this->move(availableGeometry.x() + availableGeometry.width() - this->width()-100, availableGeometry.height() - this->height());
            }else {
                this->move(availableGeometry.x() + availableGeometry.width() - this->width()-100, availableGeometry.height() - this->height() - 40);
            }
        if (!this->isHidden()) {
            this->hide();
        }
        else
            this->show();


        break;
    }
    //鼠标左键双击图标
    case QSystemTrayIcon::DoubleClick: {
        this->hide();
        break;
    }
    case QSystemTrayIcon::Context: {
//        if (!this->isHidden()) {
//            this->hide();
//        }
        menu->show();
        break;
    }
    default:
        break;
    }
}



void MainWindow::initData()
{
    saving = false;
    healthing = false;
    pressQss = "QLabel{background-color:#3593b5;}";
    releaseQss = "QLabel{background-color:#283138;}";
}

void MainWindow::initUi()
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
//    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::SplashScreen);

//    setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
    resize(230+1,150+1);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    actWidget = new PowerInteract(this);
//    setCentralWidget(actWidget);
//    this->setContentsMargins(0,10,10,0);
    setWindowOpacity(0.95);

    scroll_area = new QScrollArea(ui->centralWidget);
    scroll_area->move(1, 16);
    scroll_area->resize(230, 148-16-61);
    scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_area->show();
    pow_widget = new QWidget(scroll_area);


//    pressQss = "QLabel{background-color:#283138;}";
//    releaseQss = "QLabel{background-color:#3593b5;}";

    ui->brightlb->setStyleSheet("QLabel{background-color:#283138;}");
    ui->savelb->setStyleSheet("QLabel{background-color:#283138;}");
    ui->healthlb->setStyleSheet("QLabel{background-color:#283138;}");

    ui->centralWidget->setStyleSheet("{border:1px solid #626c6e;background-color:#151a1e;}");

    ui->power_title->setText(tr("电源管理"));
    ui->power_title->setStyleSheet("{background-color:rgba(8,10,12,0.6);}");

    scroll_area->setStyleSheet("QScrollArea{border:none;}");
    scroll_area->viewport()->setStyleSheet("background-color:transparent;");


    pow_widget->setStyleSheet("{font-size:12px;color:white;border:1px solid rgba(255,255,255,0.1);background:transparent;background-color:rgba(0,0,0,0.2);}"
                                  "Pressed{border:1px solid rgba(255,255,255,0.5);background:transparent;background-color:rgba(255,255,255,0.1);}");

    ui->brightbtn->setStyleSheet("QPushButton{border:none;}");
    ui->brighttext->setStyleSheet("QLabel{font-size:13px;color:#ffffff;}");
//    ui->brighttext->setStyleSheet("QLabel{font-size:12px;font-weight:100;color:#ffffff;}");

    ui->savebtn->setStyleSheet("QPushButton{border:none;}");
//    ui->savetext->setStyleSheet("QLabel{font-size:13px;color:#ffffff;}");
    ui->savetext->setText(tr("节能模式"));
    ui->saveicon->setStyleSheet("QLabel{background-image:url(:/22x22/apps/mate-brightness-applet.png);}");
    ui->healthicon->setStyleSheet("QLabel{background-image:url(:/22x22/apps/mate-brightness-applet.png);}");
    ui->brighticon->setStyleSheet("QLabel{background-image:url(:/22x22/apps/mate-brightness-applet.png);}");

    ui->healthbtn->setStyleSheet("QPushButton{border:none;}");
//    ui->healthtext->setStyleSheet("QLabel{background-image:url(:/res/x/setup.png);}");
    ui->healthtext->setText(tr("电池养护"));
    ui->brighttext->setText(tr("亮度"));
    get_power_list();
//    ui->brightlb->setParent(ui->brightbtn);
//    ui->healthlb->setParent(ui->healthbtn);
//    ui->savelb->setParent(ui->savebtn);
}

int MainWindow::get_engine_dev_number()
{
    QString text;
    qreal percentage;
    int len = ed->devices.size();
    int number = 0;
    for(int i = 0; i < len; i++)
    {
        DEVICE *dv;
        dv = ed->devices.at(i);
        if(dv->m_dev.kind == UP_DEVICE_KIND_LINE_POWER)
            continue;
        QString icon_name = ed->engine_get_device_icon(dv);// = get_devie_icon(deviceNames.at(i),percentage);
        percentage = dv->m_dev.Percentage;
        icon_name = ":/status/" + icon_name + ".png";
        bool is_charging = false;
        /* generate the image */
        if (icon_name.contains("charging"))
            is_charging = true;
        /* generate the label */
        if (is_charging)
        {
            text = QString("%1% available power").arg(percentage);
            text.append("\n");
            text.append("(The power is connected and is charging)");
        }
        else
            text = QString("%1% available power").arg(percentage);
        number++;
    }
}
void MainWindow::get_power_list()
{
    // chushihua
    pow_widget = new QWidget(scroll_area);
    pow_widget->resize(230, 148-16-61);
    scroll_area->setWidget(pow_widget);

    int j = 0;
    int ht = pow_widget->height();
//    int size = 2;
    int size = get_engine_dev_number();
    if(size==0)
        size = 1;

    resize(230+1,88 + size*61);
    scroll_area->resize(230, 88 + size*61-16-61);
    scroll_area->move(1, 16);
    ui->brightlb->move(ui->brightlb->pos().x(),88 + size*61 - 61);
    ui->healthlb->move(ui->healthlb->pos().x(),88 + size*61 - 61);
    ui->savelb->move(ui->savelb->pos().x(),88 + size*61 - 61);
    ui->brightbtn->move(ui->brightbtn->pos().x(),88 + size*61 - 61);
    ui->healthbtn->move(ui->healthbtn->pos().x(),88 + size*61 - 61);
    ui->savebtn->move(ui->savebtn->pos().x(),88 + size*61 - 61);
    ui->saveicon->move(ui->saveicon->pos().x(),88 + size*61 - 61 +6);
    ui->savetext->move(ui->savetext->pos().x(),88 + size*61 - 61 + 38);

    ui->healthicon->move(ui->healthtext->pos().x(),88 + size*61 - 61 +6);
    ui->healthtext->move(ui->healthtext->pos().x(),88 + size*61 - 61 + 38);
    ui->brighticon->move(ui->brighticon->pos().x(),88 + size*61 - 61 +6);
    ui->brighttext->move(ui->brighttext->pos().x(),88 + size*61 - 61 + 38);

    pow_widget->resize(230, size*61);

    size = ed->devices.size();
    for(int i = 0; i < size; i++)
    {
        DEVICE *dv;
        dv = ed->devices.at(i);
        if(dv->m_dev.kind == UP_DEVICE_KIND_LINE_POWER)
            continue;
        QString icon_name = ed->engine_get_device_icon(dv);
        icon_name = ":/22x22/status/" + icon_name + ".png";
        qDebug()<<"sdfdfsd-----"<<icon_name;
//        double percentage = dv->m_dev.Percentage;
        QString percentage = QString::number(dv->m_dev.Percentage, 'f', 1)+"%";
        bool is_charging = false;
        QString text;
        if(icon_name.contains("charging"))
            is_charging = true;
        /* generate the label */
        if (is_charging)
        {
            //                text = QString("%1% available power\n(The power is connected and is charging)").arg(percentage);
            text = QString("%1% available power").arg(percentage);
            text.append("\n");
            text.append("(The power is connected and is charging)");
        }
        else
            text = QString("%1% available power").arg(percentage);

        QString state_text = ed->engine_get_state_text(dv->m_dev.State);




        DeviceWidget *dw= new DeviceWidget(pow_widget);
        device_items.append(dw);
        dw->setIcon(icon_name);
        dw->setPercent(percentage);
        dw->setState(state_text);
        dw->setRemain("3 hours");
        dw->move(0, 0 + j * 60);
        dw->show();
//        ht += pow_widget->height() + 60;
        j++;
    }

    pow_widget->show();
}
void MainWindow::iconThemeChanged()
{
    qDebug()<<"icon theme changed";
//    QVariant var = setting->get(POWER_SCHEMA_KEY);
//    QString value = var.value<QString>();
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (QApplication::activeWindow() != this) {
            this->hide();
        }
    }
    return QWidget::event(event);
}

MainWindow::~MainWindow()
{
}

//void MainWindow::create_actions()
//{
//    QMenu *help_menu = new QMenu;

//    QAction *action_help = new QAction(this);

//    QAction *action_quit = new QAction(this);

//    QAction *action_about = new QAction(this);

//    QAction *action_setting = new QAction(this);

//    help_menu->setIcon(QIcon(":/22x22/status/gpm-ac-adapter.png"));
//    action_quit->setIcon(QIcon(":/22x22/status/gpm-ac-adapter.png"));
//    action_setting->setIcon(QIcon(":/22x22/status/gpm-ac-adapter.png"));
//    action_about->setIcon(QIcon(":/22x22/status/gpm-ac-adapter.png"));

//    this->addAction(action_help);
//    this->addAction(action_quit);
//    addAction(action_about);
//    addAction(action_setting);


//}

void MainWindow::on_savebtn_pressed()
{
    saving = !saving;
    if(saving)
    {
        ui->savelb->setStyleSheet(pressQss);
    }
    else {
        ui->savelb->setStyleSheet(releaseQss);
    }
}

void MainWindow::on_savebtn_released()
{
//    ui->savelb->setStyleSheet(releaseQss);
}

void MainWindow::on_healthbtn_pressed()
{
    healthing = ! healthing;
    if(healthing)
    {
        ui->healthlb->setStyleSheet(pressQss);
    }
    else {
        ui->healthlb->setStyleSheet(releaseQss);
    }
}

void MainWindow::on_healthbtn_released()
{
//    ui->healthlb->setStyleSheet(releaseQss);

}

void MainWindow::on_brightbtn_pressed()
{
//    ui->brightlb->setStyleSheet(pressQss);
}

void MainWindow::on_brightbtn_released()
{
//    ui->brightlb->setStyleSheet(releaseQss);

}
