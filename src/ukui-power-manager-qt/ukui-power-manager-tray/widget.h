#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
//#include <QSystemTrayIcon>
#include <QtWidgets>
#include <QDBusObjectPath>
#include <QDBusMessage>
#include "customtype.h"
#include <QDBusConnection>
#include "enginedevice.h"

//#define DBUS_SERVICE "org.freedesktop.UPower"
//#define DBUS_OBJECT "/org/freedesktop/UPower"
//#define DBUS_INTERFACE "org.freedesktop.DBus.Properties"
//#define DBUS_INTERFACE_PARAM "org.freedesktop.UPower.Device"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
//    QString get_devie_icon(QDBusObjectPath path, qreal &percentage);
//    void getDevices();
    char *gpm_upower_get_device_icon_index(qreal percentage);
public slots:
    void onActivatedIcon(QSystemTrayIcon::ActivationReason);
    void onActivatedIcon2(QSystemTrayIcon::ActivationReason reason);
    void on_icon_changed(QString str);
    void on_sum_changed(QString str);
private:
    QSystemTrayIcon *systemTrayIcon;
    QList<QDBusObjectPath> deviceNames;
    QMenu *menu;
public:
    EngineDevice* ed;

};

#endif // WIDGET_H
