#include "customtype.h"
#include <libintl.h>
#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


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

/**
 * egg_precision_round_up:
 * @value: The input value
 * @smallest: The smallest increment allowed
 *
 * 101, 10	110
 * 95,  10	100
 * 0,   10	0
 * 112, 10	120
 * 100, 10	100
 **/
int precision_round_up (float value, int smallest)
{
    float division;
    if (fabs (value) < 0.01)
        return 0;
    if (smallest == 0) {
        printf ("divisor zero");
        return 0;
    }
    division = (float) value / (float) smallest;
    division = ceilf (division);
    division *= smallest;
    return (int) division;
}

/**
 * egg_precision_round_down:
 * @value: The input value
 * @smallest: The smallest increment allowed
 *
 * 101, 10	100
 * 95,  10	90
 * 0,   10	0
 * 112, 10	110
 * 100, 10	100
 **/
int precision_round_down (float value, int smallest)
{
    float division;
    if (fabs (value) < 0.01)
        return 0;
    if (smallest == 0) {
        printf("divisor zero");
        return 0;
    }
    division = (float) value / (float) smallest;
    division = floorf (division);
    division *= smallest;
    return (int) division;
}
