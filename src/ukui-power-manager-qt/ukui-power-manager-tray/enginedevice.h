#ifndef ENGINEDEVICE_H
#define ENGINEDEVICE_H

#include <QObject>
#include <device.h>
#include "customtype.h"
#include <QString>

#define DBUS_SERVICE "org.freedesktop.UPower"
#define DBUS_OBJECT "/org/freedesktop/UPower"
#define DBUS_INTERFACE "org.freedesktop.UPower"

#define DBUS_INTERFACE_PRO "org.freedesktop.DBus.Properties"
#define DBUS_INTERFACE_DEV "org.freedesktop.UPower.Device"
class EngineDevice : public QObject
{
    Q_OBJECT
private:
    static EngineDevice* instance;
    explicit EngineDevice(QObject *parent = nullptr);
    class  Deconstructor
    {
    public:
         ~Deconstructor() {
            if(instance)
            {
                delete instance;
                instance = nullptr;
            }
        }
    };
    static Deconstructor deconstructor;

public:
    static EngineDevice* getInstance()
    {
        if(instance==nullptr)
        {
            instance = new EngineDevice;

        }
        return instance;
    }

Q_SIGNALS:
    void icon_changed(QString);
//    void sum_changed(QString);
    void engine_signal_discharge(DEV dv);
    void engine_signal_fullycharge(DEV dv);


public slots:
    void power_device_change_callback(QDBusMessage msg, QString path);

public:
    QList<DEVICE*> devices;
    QString previous_icon;
    QString privious_sum;
    QString previous_summary;

    bool use_time_primary;
    bool time_is_accurate;

    uint			 low_percentage;
    uint			 critical_percentage;
    uint			 action_percentage;
    uint			 low_time;
    uint			 critical_time;
    uint			 action_time;
    DEVICE *composite_device;
    GpmIconPolicy icon_policy;
    bool power_device_recalculate_icon();
    void power_device_recalculate_state();
    QString engine_device_get_icon_priv(UpDeviceKind device_kind, UpDeviceLevel warning, bool use_state);
    bool gpm_engine_recalculate_summary();
    bool engine_recalculate_summary();
    void power_device_cold_plug();
    void getProperty(QString path, DEV &dev);
    QString engine_get_summary();
    QString engine_kind_to_string(UpDeviceKind type_enum);
    QString engine_kind_to_localised_text(UpDeviceKind kind, uint number);
    void power_device_get_devices();

};

#endif // ENGINEDEVICE_H
