#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    ed = EngineDevice::getInstance();
    systemTrayIcon = new QSystemTrayIcon(this);
    systemTrayIcon->setIcon(QIcon(":/pro.png"));
    systemTrayIcon->show();
    connect(systemTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(onActivatedIcon(QSystemTrayIcon::ActivationReason)));
    connect(ed,SIGNAL(icon_changed(QString)),this,SLOT(on_icon_changed(QString)));
    connect(ed,SIGNAL(engine_signal_summary_change(QString)),this,SLOT(on_sum_changed(QString)));
    setFixedWidth(100);
    setWindowFlags(Qt::FramelessWindowHint);
}

Widget::~Widget()
{

}

void Widget::on_sum_changed(QString str)
{
    systemTrayIcon->setToolTip(str);
    qDebug()<<str;
}

void Widget::on_icon_changed(QString str)
{
    str = ":/status/"+str+".png";
    QIcon icon = QIcon(str);
    systemTrayIcon->setIcon(icon);;
    qDebug()<<str;
}

void Widget::onActivatedIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    {
        QPushButton *btn_pref = new QPushButton(this);
        QMenu *menu = new QMenu;
        btn_pref->setText("preferences");
        QVBoxLayout *vbox1 = new QVBoxLayout;
        QVBoxLayout *vbox2 = new QVBoxLayout;
        QString text;
//        getDevices();
        qreal percentage;
        int len = ed->devices.size();
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
//                text = QString("%1% available power\n(The power is connected and is charging)").arg(percentage);
                text = QString("%1% available power").arg(percentage);
                text.append("\n");
                text.append("(The power is connected and is charging)");
            }
            else
                text = QString("%1% available power").arg(percentage);
    //        QLabel *label =new QLabel(text);
    //        QIcon *icon = new QIcon(icon_name);
    //        QHBoxLayout *hbox = new QHBoxLayout;
            QAction *action = new QAction(QIcon(icon_name),text,this);
            menu->addAction(action);

        }
        QAction *powerSelect = new QAction(QIcon(":/pro.png"),"Preference",menu);
        menu->addAction(powerSelect);

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





void Widget::onActivatedIcon2(QSystemTrayIcon::ActivationReason reason)
{

    menu = new QMenu;
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    {
        QAction *powerItems = new QAction(QIcon(":/pro.png"),"aaaaa",menu);
//        connect(powerItems,SIGNAL(triggered()),this,SLOT(onShow()));
        QAction *powerSelect = new QAction(QIcon(":/pro.png"),"bbbbb",menu);
//        connect(powerSelect,SIGNAL(triggered()),this,SLOT(control_center_power()));
//        powerItems->setDefaultWidget(labelPowerItems);
//        powerSelect->setDefaultWidget(labelPowerSelect);

        menu->addAction(powerItems);
        menu->addAction(powerSelect);



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

