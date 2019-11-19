#include "widget.h"
#include <QApplication>
#include "eggunique.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    EggUnique uniq("2435-8667-7885-0379");
    if(!uniq.tryToRun())
    {
        qDebug()<<"program is already running";
        return 0;
    }
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
