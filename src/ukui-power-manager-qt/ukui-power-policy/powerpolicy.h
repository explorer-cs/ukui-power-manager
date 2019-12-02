#ifndef POWERPOLICY_H
#define POWERPOLICY_H

#include <QObject>
#include <QList>
#include "dbus_struct.h"
#include <QDBusMessage>
#include "gsettings.h"

class PowerPolicy : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","ukui.power.policy")
public:
    explicit PowerPolicy(QObject *parent = 0);

    int process(int option);
    int script_process(QString cmd);
Q_SIGNALS:
    void onbattery_change(bool);
    void ModeChanged(int);
public Q_SLOTS:
    void set_integer(int argc);
    void set_string(QString argc);

    int           return_integer();
    bool          return_bool();
    QString       return_string();
    QVariantList  return_variantlist();
    int 		  return_integet_and_set_integer(int argc_1, const QString &argc_2, QString &out1);
    QString 	  return_string_and_set_string(const QString &argc_1);
    QString           control(int opt);
    void onPropertiesSlot(QDBusMessage msg);
    void onbattery_change_slot(bool flag);
    int power_control(QString power_status, QString power_mode);
private:
    int dbus_integer;
    QString ret;
    QVariant dbus_variant;
    bool onbattery;
    int mode;
//    QGSettings *settings;
};

#endif // POWERPOLICY_H
