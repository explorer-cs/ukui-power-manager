#ifndef POWERPOLICY_H
#define POWERPOLICY_H

#include <QObject>
#include <QList>
#include "dbus_struct.h"

class PowerPolicy : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","tlp.local.manager")
public:
    explicit PowerPolicy(QObject *parent = nullptr);

    int process(char *option);
signals:

public slots:
    void set_integer(int argc);
    void set_string(QString argc);
    void set_variantlist(int cnt, QVariantList argc);

    int           return_integer();
    bool          return_bool();
    QString       return_string();
    QVariantList  return_variantlist();
    int 		  return_integet_and_set_integer(int argc_1, const QString &argc_2, QString &out1);
    QString 	  return_string_and_set_string(const QString &argc_1);
    QString           control_tlp(int opt);
private:
    int dbus_integer;
    QString ret;
    QVariant dbus_variant;

};

#endif // POWERPOLICY_H
