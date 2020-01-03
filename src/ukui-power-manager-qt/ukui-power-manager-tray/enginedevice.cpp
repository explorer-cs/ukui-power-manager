#include "enginedevice.h"
#include "device.h"
#include <QDBusObjectPath>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDebug>

#include "engine-common.h"

EngineDevice* EngineDevice::instance = NULL;

void EngineDevice::power_device_get_devices()
{

    QList<QDBusObjectPath> deviceNames;

    /* call enum dbus*/
    QDBusMessage msg = QDBusMessage::createMethodCall(DBUS_SERVICE,DBUS_OBJECT,
            DBUS_INTERFACE,"EnumerateDevices");
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
        DEVICE *device = new DEVICE;
        device->m_dev.path = deviceNames.at(i).path();
        getProperty(device->m_dev.path,device->m_dev);
        /* connect the nofity signal to changecallback */
        QDBusConnection::systemBus().connect(DBUS_SERVICE,device->m_dev.path,DBUS_INTERFACE_PRO,
                                             QString("PropertiesChanged"),device,SLOT(handleChanged(QDBusMessage)));
        connect(device,SIGNAL(device_property_changed(QDBusMessage,QString)),this,SLOT(power_device_change_callback(QDBusMessage,QString)));

        /* add to device list*/
        devices.append(device);
    }

}


EngineDevice::EngineDevice(QObject *parent) : QObject(parent)
{
    power_device_get_devices();
    QDBusConnection::systemBus().connect(DBUS_SERVICE,DBUS_OBJECT,DBUS_SERVICE,
                                         QString("device-added"),this,SLOT(power_device_add(QDBusMessage)));
    QDBusConnection::systemBus().connect(DBUS_SERVICE,DBUS_OBJECT,DBUS_SERVICE,
                                         QString("device-removed"),this,SLOT(power_device_remove(QDBusMessage)));
}

void EngineDevice::power_device_add(QDBusMessage msg)
{
    UpDeviceState state;
    UpDeviceKind kind;

    QDBusObjectPath objectPath;
    const QDBusArgument &arg = msg.arguments().at(0).value<QDBusArgument>();
    arg >> objectPath;

    QDBusMessage msgTmp = QDBusMessage::createMethodCall(DBUS_SERVICE,objectPath.path(),
            "org.freedesktop.DBus.Properties","GetAll");
    msgTmp << DBUS_INTERFACE_DEV;
    QDBusMessage res = QDBusConnection::systemBus().call(msgTmp);
    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        QMap<QString,QVariant> map;
        dbusArg >> map;
        UpDeviceKind kind = (UpDeviceKind)map.value("Type").toInt();

        if(kind != UP_DEVICE_KIND_LINE_POWER && kind != UP_DEVICE_KIND_BATTERY && kind != UP_DEVICE_KIND_COMPUTER)
            return;

        DEVICE *dev = new DEVICE;
        dev->m_dev.path = objectPath.path();
        dev->m_dev.kind = kind;
        dev->m_dev.warnlevel = (UpDeviceLevel)map.value("WarningLevel").toUInt();
        dev->m_dev.Capacity = map.value(QString("Capacity")).toDouble();
        dev->m_dev.State = (UpDeviceState)map.value("State").toUInt();

        /*add to array*/
        devices.append(dev);
        QDBusConnection::systemBus().connect(DBUS_SERVICE,dev->m_dev.path,DBUS_INTERFACE_PRO,
                                             QString("PropertiesChanged"),dev,SLOT(handleChanged(QDBusMessage)));
        /*connect notify signals*/
        connect(dev,SIGNAL(device_property_changed(QDBusMessage,QString)),this,SLOT(power_device_change_callback(QDBusMessage,QString)));
        /*recaculate state*/

    }
}

void EngineDevice::power_device_remove(QDBusMessage msg)
{
    foreach (auto item, devices)
    {
        if(item->m_dev.path == msg.path())
        //list.removed
        {
            devices.removeOne(item);
            delete item;
            item = NULL;
            break;
        }


    }

}

void EngineDevice::getProperty(QString path,DEV& dev)
{

    QDBusMessage msg = QDBusMessage::createMethodCall(DBUS_SERVICE,path,
            DBUS_INTERFACE_PRO,"GetAll");
    msg << DBUS_INTERFACE_DEV;
    QDBusMessage res = QDBusConnection::systemBus().call(msg);

    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        QMap<QString,QVariant> map;
        dbusArg >> map;

        dev.kind = (UpDeviceKind)map.value(QString("Type")).toInt();
        dev.Type = engine_kind_to_localised_text ((UpDeviceKind)map.value(QString("Type")).toInt(),1);
        dev.Model = map.value(QString("Model")).toString();
        dev.Device = map.value(QString("NativePath")).toString();
//        dev.Vendor = map.value(QString("Vendor")).toString();
//        dev.Capacity = QString::number(map.value(QString("Capacity")).toDouble(), 'f', 1) + "%";
        dev.Capacity = (map.value(QString("Capacity")).toDouble(), 'f', 1);
        dev.Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
        dev.EnergyEmpty= QString::number(map.value(QString("EnergyEmpty")).toDouble(), 'f', 1)+ " Wh";
        dev.EnergyFull = QString::number(map.value(QString("EnergyFull")).toDouble(), 'f', 1)+ " Wh";
//        dev.EnergyFullDesign = QString::number(map.value(QString("EnergyFullDesign")).toDouble(), 'f', 1) + " Wh";
        dev.EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
        dev.IsPresent = (map.value(QString("IsPresent")).toBool());
//        dev.IsRechargeable = boolToString(map.value(QString("IsRechargeable")).toBool());
        dev.PowerSupply = boolToString(map.value(QString("PowerSupply")).toBool());
        dev.Percentage = map.value(QString("Percentage")).toDouble();
        dev.Percentage = ( (float)( (int)( (dev.Percentage + 0.05) * 10 ) ) ) / 10;

        dev.Online = boolToString(map.value(QString("Online")).toBool());



        dev.TimeToEmpty = map.value(QString("TimeToEmpty")).toLongLong();
        dev.TimeToFull = map.value(QString("TimeToFull")).toLongLong();
        dev.Voltage = QString::number(map.value(QString("Voltage")).toDouble(), 'f', 1) + " V";

    }
}

QString EngineDevice::boolToString(bool ret)
{
    return ret ? tr("yes") : tr("no");
}


void EngineDevice::putAttributes(QMap<QString,QVariant>& map,DEV &btrDetailData)
{
    if(map.contains("TimeToFull"))
    {
        btrDetailData.TimeToFull= map.value(QString("TimeToFull")).toLongLong();
    }
    if(map.contains("TimeToEmpty"))
        btrDetailData.TimeToEmpty = map.value(QString("TimeToEmpty")).toLongLong();
    if(map.contains("EnergyRate"))
        btrDetailData.EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
    if(map.contains("Energy"))
        btrDetailData.Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
    if(map.contains("Voltage"))
        btrDetailData.Voltage = QString::number(map.value(QString("Voltage")).toDouble(), 'f', 1) + " V";
    if(map.contains("WarnLevel"))
    {
        btrDetailData.warnlevel = (UpDeviceLevel)map.value(QString("WarnLevel")).toInt();
    }

    if(map.contains("State"))
    {
        btrDetailData.State = (UpDeviceState)map.value(QString("State")).toInt();
    }
    if(map.contains("Percentage"))
    {
        btrDetailData.Percentage = map.value(QString("Percentage")).toDouble();
        btrDetailData.Percentage = ( (float)( (int)( (btrDetailData.Percentage + 0.05) * 10 ) ) ) / 10;

    }

    if(map.contains("PowerSupply"))
        btrDetailData.PowerSupply = (map.value(QString("PowerSupply")).toBool()) ? tr("Yes") :tr("No");
}

void EngineDevice::power_device_change_callback(QDBusMessage msg,QString path)
{
    /* if battery change to display devices */
    /* judge state */
    qDebug()<<"change callback-------";
    DEVICE *item = nullptr;
    foreach (item, devices)
    {
        if(item->m_dev.path == path)
        {
            break;
        }
        return;
    }

    DEV tmp_dev = item->m_dev;
    const QDBusArgument &arg = msg.arguments().at(1).value<QDBusArgument>();
    QMap<QString,QVariant> map;
    arg >> map;
    putAttributes(map,item->m_dev);
//    getProperty(path,tmp_dev);
    if(item->m_dev.State != tmp_dev.State)
    {
        if(item->m_dev.State==UP_DEVICE_STATE_DISCHARGING)
        {
            Q_EMIT engine_signal_discharge(tmp_dev);
        }
        else if (item->m_dev.State==UP_DEVICE_STATE_FULLY_CHARGED)
        {
            Q_EMIT engine_signal_fullycharge(tmp_dev);
        }
        //save new state
    }
    /* check the warning state has not changed */
    if(item->m_dev.warnlevel != tmp_dev.warnlevel)
    {
        if(item->m_dev.warnlevel==UP_DEVICE_LEVEL_LOW)
        {
            Q_EMIT engine_signal_charge_low(tmp_dev);
        }
        else if (item->m_dev.warnlevel==UP_DEVICE_LEVEL_CRITICAL)
        {
            Q_EMIT engine_signal_charge_critical(tmp_dev);
        }
        else if(item->m_dev.warnlevel==UP_DEVICE_LEVEL_ACTION)
        {
            Q_EMIT engine_signal_charge_action(tmp_dev);
        }
        //save new warning;
    }

    /*recaculate state*/
    power_device_recalculate_state();
}

void EngineDevice::power_device_recalculate_state()
{

    /*recaculate icon*/
    power_device_recalculate_icon();
    /*recaculate sum*/
    engine_recalculate_summary();
}

bool EngineDevice::power_device_recalculate_icon()
{
    /*call a function to obtain icon*/
    QString icon;
    icon = power_device_get_icon();
    if(icon.isNull())
    {
        if(previous_icon.isNull())
            return false;
        emit icon_changed(QString());
        previous_icon.clear();
        return true;
    }
    if(previous_icon.isNull())
    {
        emit icon_changed(icon);
        previous_icon = icon;
        return true;
    }

    if(previous_icon != icon)
    {
        emit icon_changed(icon);
        previous_icon = icon;
        return true;
    }
    return false;
}

/**
 * power_device_get_icon:
 *
 * Returns the icon
 **/


/**
 *
 * Returns the icon
 **/
QString EngineDevice::engine_device_get_icon_priv (UpDeviceKind device_kind, UpDeviceLevel warning, bool use_state)
{
    uint i;
    DEVICE *device;
    UpDeviceLevel warning_temp;
    UpDeviceKind kind;
    UpDeviceState state;
    bool is_present;
    foreach (device, devices) {
        state = device->m_dev.State;
        if ((device->m_dev.kind == device_kind) && (device->m_dev.IsPresent)) {
            if (warning != UP_DEVICE_LEVEL_NONE) {
                if (warning_temp == warning) {
                    return engine_get_device_icon (device);
                }
                continue;
            }
            if (use_state) {
                if (state == UP_DEVICE_STATE_CHARGING || state == UP_DEVICE_STATE_DISCHARGING) {
                    return engine_get_device_icon (device);
                }

                continue;
            }
            return engine_get_device_icon (device);
        }
    }
    return NULL;
}

/**
 */
bool EngineDevice::engine_recalculate_summary ()
{
    QString summary;

    summary = engine_get_summary ();
    if (previous_summary.isNull()) {
        previous_summary = summary;
        Q_EMIT engine_signal_summary_change(summary);
        return true;
    }

    if (previous_summary != summary) {
        previous_summary = summary;
        Q_EMIT engine_signal_summary_change(summary);
        return true;
    }
    printf ("no change");
    return false;
}

/**
 * engine_get_summary:
 **/
QString EngineDevice::engine_get_summary ()
{
    DEVICE *device;
    UpDeviceState state;
    QString tooltip;
    QString part;
    bool is_present;

    foreach (device, devices) {

        is_present = device->m_dev.IsPresent;
        state = device->m_dev.State;
        if (!is_present)
            continue;
        if (state == UP_DEVICE_STATE_EMPTY)
            continue;
        part = engine_get_device_summary (device);
        if (!part.isNull())
            tooltip = QString("%1\n").arg(part);
    }
    return tooltip;

}


QString EngineDevice::engine_kind_to_string (UpDeviceKind type_enum)
{
    QString ret;
    switch (type_enum) {
    case UP_DEVICE_KIND_LINE_POWER:
        ret = "line-power";
        break;
    case UP_DEVICE_KIND_BATTERY:
        ret = "battery";
        break;
    case UP_DEVICE_KIND_UPS:
        ret = "ups";
        break;
    case UP_DEVICE_KIND_MONITOR:
        ret = "monitor";
        break;
    case UP_DEVICE_KIND_MOUSE:
        ret = "mouse";
        break;
    case UP_DEVICE_KIND_KEYBOARD:
        ret = "keyboard";
        break;
    case UP_DEVICE_KIND_PDA:
        ret = "pda";
        break;
    case UP_DEVICE_KIND_PHONE:
        ret = "phone";
        break;
    case UP_DEVICE_KIND_MEDIA_PLAYER:
        ret = "media-player";
        break;
    case UP_DEVICE_KIND_TABLET:
        ret = "tablet";
        break;
    case UP_DEVICE_KIND_COMPUTER:
        ret = "computer";
        break;
    default:
        ret = "unknown";
    }
    return ret;
}

/**
 * engine_kind_to_localised_text:
 **/
QString EngineDevice::engine_kind_to_localised_text (UpDeviceKind kind, uint number)
{
    Q_UNUSED(number);
    QString text;
    switch (kind) {
    case UP_DEVICE_KIND_LINE_POWER:
        /* TRANSLATORS: system power cord */
//        text = ngettext ("AC adapter", "AC adapters", number);
        text =  ("AC adapter");
        break;
    case UP_DEVICE_KIND_BATTERY:
        /* TRANSLATORS: laptop primary battery */
        text =  ("Laptop battery");
        break;
    case UP_DEVICE_KIND_UPS:
        /* TRANSLATORS: battery-backed AC power source */
        text =  ("UPS");
        break;
    case UP_DEVICE_KIND_MONITOR:
        /* TRANSLATORS: a monitor is a device to measure voltage and current */
        text =  ("Monitor");
        break;
    case UP_DEVICE_KIND_MOUSE:
        /* TRANSLATORS: wireless mice with internal batteries */
        text =  ("Mouse");
        break;
    case UP_DEVICE_KIND_KEYBOARD:
        /* TRANSLATORS: wireless keyboard with internal battery */
        text =  ("Keyboard");
        break;
    case UP_DEVICE_KIND_PDA:
        /* TRANSLATORS: portable device */
        text =  ("PDA");
        break;
    case UP_DEVICE_KIND_PHONE:
        /* TRANSLATORS: cell phone (mobile...) */
        text =  ("Cell phone");
        break;
    case UP_DEVICE_KIND_MEDIA_PLAYER:
        /* TRANSLATORS: media player, mp3 etc */
        text =  ("Media player");
        break;
    case UP_DEVICE_KIND_TABLET:
        /* TRANSLATORS: tablet device */
        text =  ("Tablet");
        break;
    case UP_DEVICE_KIND_COMPUTER:
        /* TRANSLATORS: tablet device */
        text =  ("Computer");
        break;
    default:
        printf ("enum unrecognised: %i", kind);
//        text = engine_kind_to_string (kind);
    }
    return text;
}


QString EngineDevice::engine_get_device_icon_index (qreal percentage)
{
    if (percentage < 10)
        return "000";
    else if (percentage < 30)
        return "020";
    else if (percentage < 50)
        return "040";
    else if (percentage < 70)
        return "060";
    else if (percentage < 90)
        return "080";
    return "100";
}

/**
 * gpm_upower_get_device_icon:
 *
 * Need to free the return value
 *
 **/
QString EngineDevice::engine_get_device_icon (DEVICE *device)
{
    QString filename;
    QString prefix;
    QString index_str;
    UpDeviceKind kind;
    UpDeviceState state;
    bool is_present;
    QString result;
    qreal percentage;

    kind = device->m_dev.kind;
    state = device->m_dev.State;
    percentage = device->m_dev.Percentage;
    is_present = device->m_dev.IsPresent;
    /* get correct icon prefix */
    prefix = engine_kind_to_string (kind);

    /* get the icon from some simple rules */
    if (kind == UP_DEVICE_KIND_LINE_POWER) {
        result =  ("gpm-ac-adapter");
    } else if (kind == UP_DEVICE_KIND_MONITOR) {
        result =  ("gpm-monitor");
    } else if (kind == UP_DEVICE_KIND_UPS) {
        if (!is_present) {
            /* battery missing */
            result.sprintf ("gpm-%s-missing", prefix);

        } else if (state == UP_DEVICE_STATE_FULLY_CHARGED) {
            result.sprintf ("gpm-%s-100", prefix);

        } else if (state == UP_DEVICE_STATE_CHARGING) {
            index_str = engine_get_device_icon_index (percentage);
            result = QString("gpm-%1-%2-charging").arg(prefix).arg(index_str);

        } else if (state == UP_DEVICE_STATE_DISCHARGING) {
            index_str = engine_get_device_icon_index (percentage);
            result = QString("gpm-%1-%2").arg(prefix).arg(index_str);
        }
    } else if (kind == UP_DEVICE_KIND_BATTERY) {
        if (!is_present) {
            /* battery missing */
            result = QString ("gpm-%1-missing").arg(prefix);

        } else if (state == UP_DEVICE_STATE_EMPTY) {
            result = QString ("gpm-%1-empty").arg(prefix);

        } else if (state == UP_DEVICE_STATE_FULLY_CHARGED) {
                    filename = QString ("gpm-%1-charged").arg(prefix);
        } else if (state == UP_DEVICE_STATE_CHARGING) {
            index_str = engine_get_device_icon_index (percentage);
            result = QString("gpm-%1-%2-charging").arg(prefix).arg(index_str);

        } else if (state == UP_DEVICE_STATE_DISCHARGING) {
            index_str = engine_get_device_icon_index (percentage);
            result = QString("gpm-%1-%2").arg(prefix).arg(index_str);

        } else if (state == UP_DEVICE_STATE_PENDING_CHARGE) {
            index_str = engine_get_device_icon_index (percentage);
            /* FIXME: do new grey icons */
            result = QString("gpm-%1-%2-charging").arg(prefix).arg(index_str);

        } else if (state == UP_DEVICE_STATE_PENDING_DISCHARGE) {
            index_str = engine_get_device_icon_index (percentage);
            result = QString("gpm-%1-%2").arg(prefix).arg(index_str);

        } else {
            result =  ("gpm-battery-missing");
        }

    } else if (kind == UP_DEVICE_KIND_MOUSE ||
           kind == UP_DEVICE_KIND_KEYBOARD ||
           kind == UP_DEVICE_KIND_PHONE) {
        if (!is_present) {
            /* battery missing */
            result = QString ("gpm-%1-000").arg(prefix);

        } else if (state == UP_DEVICE_STATE_FULLY_CHARGED) {
            result = QString ("gpm-%1-100").arg(prefix);

        } else if (state == UP_DEVICE_STATE_DISCHARGING) {
            index_str = engine_get_device_icon_index (percentage);
            result = QString("gpm-%1-%2").arg(prefix).arg(index_str);

        }
    }

    /* nothing matched */
    if (result.isNull()) {
        printf ("nothing matched, falling back to default icon");
        result =  ("dialog-warning");
    }
    qDebug()<<result;
    return result;
}
