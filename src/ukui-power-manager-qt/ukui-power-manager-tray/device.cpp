#include "device.h"
#include <QDebug>

DEVICE::DEVICE(QObject *parent) : QObject(parent)
{
}

void DEVICE::handleChanged(QDBusMessage msg)
{
//    qDebug()<<"device properties changed";
    Q_UNUSED(msg);
    Q_EMIT device_property_changed(msg,m_dev.path);
}


DEVICE::~DEVICE()
{
}
