#ifndef DEVICE_H
#define DEVICE_H

#include "statistics-common.h"
#include "QDBusMessage"

class DEVICE : public QObject
{
    Q_OBJECT
public:
    explicit DEVICE(QObject *parent = nullptr);
    ~DEVICE();
Q_SIGNALS:
    void device_property_changed(QString path);


public Q_SLOTS:
    void handleChanged(QDBusMessage msg);
public:
    DEV *m_dev;
};

#endif // DEVICE_H
