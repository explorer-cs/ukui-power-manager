#ifndef CUSTOMTYPE_H
#define CUSTOMTYPE_H

#include <QDBusMetaType>
#include <QDBusVariant>
/**
 * QDBusMetaType:
 *
 * register qdbus type.
 **/
struct StructUdu
{
    quint32 time;
    qreal value;
    quint32 state;
};

QDBusArgument &operator<<(QDBusArgument &argument, const StructUdu &structudp);
const QDBusArgument &operator>>(const QDBusArgument &argument, StructUdu &structudp);
QDBusArgument &operator<<(QDBusArgument &argument, const QList<StructUdu> &myarray);
const QDBusArgument &operator>>(const QDBusArgument &argument, QList<StructUdu> &myarray);

void registerCustomType();


Q_DECLARE_METATYPE(QList<StructUdu>)
Q_DECLARE_METATYPE(StructUdu)


#define GPM_UP_TIME_PRECISION			5*60
#define GPM_UP_TEXT_MIN_TIME			120


int precision_round_down (float value, int smallest);
int precision_round_up (float value, int smallest);


#endif // CUSTOMTYPE_H
