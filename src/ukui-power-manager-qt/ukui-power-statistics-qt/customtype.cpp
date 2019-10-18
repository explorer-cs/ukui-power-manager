#include "customtype.h"
#include <libintl.h>

void registerCustomType()
{
    qDBusRegisterMetaType<StructUdu>();
    qDBusRegisterMetaType<QList<StructUdu>>();

}


QDBusArgument &operator<<(QDBusArgument &argument, const StructUdu &structudp)
{
    argument.beginStructure();
    argument<<structudp.time<<structudp.value<<structudp.state;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, StructUdu &structudp)
{
    argument.beginStructure();
    argument>>structudp.time>>structudp.value>>structudp.state;
    argument.endStructure();
    return argument;
}


QDBusArgument &operator<<(QDBusArgument &argument, const QList<StructUdu> &myarray)
{
    argument.beginArray(qMetaTypeId<StructUdu>());
    for(int i= 0; i<myarray.length(); i++)
        argument << myarray.at(i);
    argument.endArray();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QList<StructUdu> &myarray)
{
    argument.beginArray();
    myarray.clear();
    while(!argument.atEnd())
    {
        StructUdu element;
        argument>>element;
        myarray.append(element);
    }
    argument.endArray();
    return argument;
}

