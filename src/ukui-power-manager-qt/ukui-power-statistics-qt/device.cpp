#include "device.h"
#include <QDebug>

DEVICE::DEVICE(QObject *parent) : QObject(parent)
{
    m_dev = new DEV;
}

void DEVICE::handleChanged(QDBusMessage msg)
{
//    qDebug()<<"device properties changed";
    Q_UNUSED(msg);
    Q_EMIT device_property_changed(m_dev->path);
}


DEVICE::~DEVICE()
{
    delete m_dev;
}
