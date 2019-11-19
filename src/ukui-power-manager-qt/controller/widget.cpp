#include "widget.h"
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>
#include <QDBusReply>

#define SAVEPOWER            0
#define PERFORMANCE          1
#define DEFAULT              2

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
//    QLabel *savepower = new QLabel("SavePower",this);
    status = new QLabel("Status",this);
    saveradio = new QRadioButton("SavePower",this);
    perfradio = new QRadioButton("Performance",this);
    defaultradio = new QRadioButton("Default",this);

    saveradio->setChecked(false);
    perfradio->setChecked(false);
    defaultradio->setChecked(false);
    layout->addWidget(saveradio);
    layout->addWidget(perfradio);
    layout->addWidget(defaultradio);
    layout->addWidget(status);

    setLayout(layout);
    resize(200,400);
    QDesktopWidget *deskdop = QApplication::desktop();
    move((deskdop->width() - this->width())/2, (deskdop->height() - this->height())/2);

    interface = new QDBusInterface("tlp.local.manager", "/tlp/local/manager",
                             "tlp.local.manager",
                             QDBusConnection::systemBus());
    if (!interface->isValid())
    {
        qDebug() << qPrintable(QDBusConnection::systemBus().lastError().message());
        exit(1);
    }


    connect(saveradio,SIGNAL(clicked(bool)),this,SLOT(onSaveChecked(bool)));
    connect(perfradio,SIGNAL(clicked(bool)),this,SLOT(onPerfChecked(bool)));
}

Widget::~Widget()
{
    delete interface;
}

void Widget::turnOff()
{
}

void Widget::turnOn()
{
}

void Widget::onSaveChecked(bool checked)
{
    qDebug() << "save";

    if(checked)
    {

        QDBusReply<QString> reply = interface->call("control_tlp",SAVEPOWER);
        if (reply.isValid())
        {
            QString value = reply.value();
            qDebug() << value;
            value += ": savepower";
            status->setText(value);
        }
        else
        {
            qDebug() << "value method called failed!";
        }
    }

}

void Widget::onPerfChecked(bool checked)
{
    qDebug() << "perf";

    if(checked)
    {

        QDBusReply<QString> reply = interface->call("control_tlp",PERFORMANCE);
        if (reply.isValid())
        {
            QString value = reply.value();
            qDebug() << value;
            value += ": performance";

            status->setText(value);
        }
        else
        {
            qDebug() << "value method called failed!";
        }
    }
}

void Widget::onDefaultChecked(bool checked)
{
    qDebug() << "default";

    if(checked)
    {
        QDBusReply<QString> reply = interface->call("control_tlp",DEFAULT);
        if (reply.isValid())
        {
            QString value = reply.value();
            qDebug() << value;
            value += ": default";

            status->setText(value);
        }
        else
        {
            qDebug() << "value method called failed!";
        }
    }
}
