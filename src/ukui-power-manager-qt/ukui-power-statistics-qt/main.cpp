#include <QApplication>
#include <QTranslator>
#include <eggunique.h>
#include "ukpm-widget.h"

int main(int argc, char *argv[])
{
    EggUnique uniq("4822-6fcc-4567-8334");
    if(!uniq.tryToRun())
    {
        qDebug()<<"program is already running";
        return 0;
    }
    QApplication a(argc, argv);
//    if(argc < 3)
//    {
//	qDebug()<<"this program is called by ukui-power-manager with 3 arguments totally";
//        qDebug()<<"device supported now, line_power and battery";
//        qDebug()<<"for example: ./run --device battery";
//        return 0;
//    }
//    QString dev = argv[2];
//    if(!(dev.contains("battery")) && !(dev.contains("line_power")))
//    {
//        qDebug()<<"device supported now, line_power and battery";
//        qDebug()<<"for example: ./run --device battery";
//        return 0;
//    }
    QString locale = QLocale::system().name();
    QTranslator translator;
    QString qmfile = QString(":/locale/%1.qm").arg(locale);
    translator.load(qmfile);
    a.installTranslator(&translator);
    a.setWindowIcon(QIcon(":/pro.png"));
    UkpmWidget *w = new UkpmWidget;
    w->show();
    return a.exec();
}
