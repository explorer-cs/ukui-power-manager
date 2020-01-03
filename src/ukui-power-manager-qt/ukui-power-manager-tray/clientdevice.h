#ifndef CLIENTDEVICE_H
#define CLIENTDEVICE_H

#include <QObject>

class ClientDevice : public QObject
{
    Q_OBJECT
public:
    explicit ClientDevice(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CLIENTDEVICE_H