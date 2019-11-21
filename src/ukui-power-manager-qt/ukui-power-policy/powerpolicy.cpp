#include "powerpolicy.h"
#include <stdio.h>
#include <stdlib.h>
#include "dbus_struct.h"
#include <QDBusConnection>
#include <QDBusArgument>
#include <QDebug>

#define PERFORMANCE          0
#define DEFAULT              1
#define SAVEPOWER            2

#define DBUS_SERVICE "org.freedesktop.UPower"
#define DBUS_OBJECT "/org/freedesktop/UPower"
#define DBUS_INTERFACE "org.freedesktop.DBus.Properties"
#define POWER_SETTINGS_SCHEMA "org.mate.power-manager"
#define POWERPOLICY_MODE_TEXT			"power-policy-mode"



PowerPolicy::PowerPolicy(QObject *parent) : QObject(parent)
{
    settings = new QGSettings(POWER_SETTINGS_SCHEMA);
    mode = settings->getInt(POWERPOLICY_MODE_TEXT);
//    qDebug()<<"mode is:"<<mode;
//    if(mode == 0)
//    {
//        mode = DEFAULT;
//        settings->setInt(POWERPOLICY_MODE_TEXT,mode);
//    }
    connect(this,SIGNAL(onbattery_change(bool)),this,SLOT(onbattery_change_slot(bool)));
    QDBusConnection::systemBus().connect(DBUS_SERVICE,DBUS_OBJECT,DBUS_INTERFACE,
                                         QString("PropertiesChanged"),this,SLOT(onPropertiesSlot(QDBusMessage)));
}

void PowerPolicy::onPropertiesSlot(QDBusMessage msg)
{

    const QDBusArgument &arg = msg.arguments().at(1).value<QDBusArgument>();
    QMap<QString,QVariant> map;
    arg >> map;
    if(map.contains("OnBattery"))
    {
        onbattery=map.value(QString("OnBattery")).toBool();
        Q_EMIT onbattery_change(onbattery);
    }

}

void PowerPolicy::onbattery_change_slot(bool flag)
{
    if(flag)
        qDebug()<<"power on battery";
    else
        qDebug()<<"power on ac";
   control(mode);

}

void PowerPolicy:: set_integer(int argc)
{
    printf("set_integer: %d\n", argc);
}

void PowerPolicy:: set_string(QString argc)
{
    printf("set_string: %s\n", argc.toStdString().data());
}


int PowerPolicy:: return_integer()
{
    int a1, a2;
    a1 = 10;
    a2 = 20;
    return a1 + a2;
}

bool PowerPolicy:: return_bool()
{
    bool result;
    result = true;
    return result;
}

QString PowerPolicy:: return_string()
{
    QString name = "zhangsan";
    return name;
}

QVariantList PowerPolicy::return_variantlist()
{
    QVariantList value;
    QVariant cnt;
    struct dbus_demo_example_struct demo[3];
    for(int i = 0; i < 3; i++)
    {
        demo[i].drv_ID = 1;
        demo[i].name   = "wangwu";
        demo[i].full_name = "lisi";
        demo[i].notify_mid = 2;
        cnt = QVariant::fromValue(demo[i]);
        value << cnt;
    }
    return value;
}

int PowerPolicy:: return_integet_and_set_integer(int argc_1, const QString &argc_2, QString &out1)
{
    int a;
    a = argc_1;
    out1 = argc_2;
    return a;
}

QString PowerPolicy:: return_string_and_set_string(const QString &argc_1)
{
    QString value;
    value = argc_1.toLower();
    return value;
}

QString PowerPolicy::control(int opt)
{
    if(opt == SAVEPOWER)
    {
        mode = opt;
        process(mode);

    }
    else if(opt == PERFORMANCE)
    {
        mode = opt;
        process(mode);

    }
    else if(opt == DEFAULT)
    {
        mode = opt;
        process(mode);

    }
    else
        ret.sprintf("undefined mode");
    return ret;

}

int PowerPolicy::process(int option)
{
      settings->setInt(POWERPOLICY_MODE_TEXT,mode);
      QString cmd;
      cmd.sprintf("~/tlp/myshellc/test.sh %d",option);
      int rv = system(cmd.toStdString().c_str());
        if (WIFEXITED(rv))
        {
             printf("subprocess exited, exit code: %d\n", WEXITSTATUS(rv));
             if (0 == WEXITSTATUS(rv))
             {
                  // if command returning 0 means succeed
                  printf("command succeed");
                  ret.sprintf("command succeed");
             }
             else
             {
                  if(127 == WEXITSTATUS(rv))
                  {
                       printf("command not found\n");
                       ret.sprintf("command not found");
                       return WEXITSTATUS(rv);
                  }
                  else
                  {
                       printf("command failed: %s\n", strerror(WEXITSTATUS(rv)));
                       ret.sprintf("command failed");
                       return WEXITSTATUS(rv);
                  }
             }
         }
        else
        {
             printf("subprocess exit failed");
             ret.sprintf("subprocess exit failed");
             return -1;
        }
        return -1;

}

