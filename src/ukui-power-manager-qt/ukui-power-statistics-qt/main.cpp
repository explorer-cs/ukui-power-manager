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
    QStringList arguments = QApplication::arguments();
    if(arguments.size() < 3)
    {
	    qDebug()<<"arguments is less than 3";
	    return 0;
    }
    QString dev = arguments.at(2);
    if(!(dev.contains("battery")) && !(dev.contains("line_power")))
    {
	    qDebug()<<"device not supported now, try battery";
	    return 0;
    }
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
