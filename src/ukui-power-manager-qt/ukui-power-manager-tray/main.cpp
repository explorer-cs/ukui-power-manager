#include "widget.h"
#include <QApplication>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSharedMemory mem("SingleAppl");
    if(mem.attach())
    {
        mem.detach();
    }
    QSharedMemory unimem("SingleAppl");
    bool is_run=false;
    if(unimem.attach())
    {
        is_run = true;
    }
    else
    {
        unimem.create(1);
        is_run = false;
    }
    if(is_run)
    {
        qWarning()<<QStringLiteral("program is already running! exit!");
        exit(0);
    }

    Widget w;
//    w.show();

    return a.exec();
}
