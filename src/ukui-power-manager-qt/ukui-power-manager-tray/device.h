#ifndef DEVICE_H
#define DEVICE_H

#include <QDBusMessage>
//#include </usr/include/libupower-glib/up-types.h>
#include "engine-common.h"

struct DEV
{
    UpDeviceKind kind;
    UpDeviceLevel warnlevel;
    QString Device;
    QString Type;
    QString PowerSupply;
    QString Online;
    QString Vendor;
    QString Model;
    QString Refresh;
    QString Energy;
    QString EnergyEmpty;
    QString EnergyFull;
    QString EnergyFullDesign;
    QString EnergyRate;
    bool IsPresent;
    QString IsRechargeable;
    double Percentage;
    UpDeviceState State;
    uint TimeToEmpty;
    uint TimeToFull;
    QString Voltage;
    double Capacity;
    QString update_time;
    QString serial;
    QString Technology;
    bool hasHistory;
    bool hasStat;
    QString path;
//    uint warnlevel;

    bool autorange_x;
    int start_x;
    int stop_x;
    bool autorange_y;
    int start_y;
    int stop_y;
};

class DEVICE : public QObject
{
    Q_OBJECT
public:
    explicit DEVICE(QObject *parent = nullptr);
    ~DEVICE();
Q_SIGNALS:
    void device_property_changed(QDBusMessage msg,QString path);


public Q_SLOTS:
    void handleChanged(QDBusMessage msg);
public:
    DEV m_dev;
};

#endif // DEVICE_H
