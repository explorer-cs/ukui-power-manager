#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QSharedMemory>
#include <QDebug>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSharedMemory mem("SingleApp-ukui-power-manager-tray");
    if(mem.attach())
    {
        mem.detach();
    }
    QSharedMemory unimem("SingleApp-ukui-power-manager-tray");
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
    QString locale = QLocale::system().name();
    QTranslator translator;
    QString qmfile = QString(":/%1.qm").arg(locale);
    qDebug()<<qmfile;
    translator.load(qmfile);
    a.installTranslator(&translator);
    QFile file(":/main.qss");
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(file.readAll());
    file.close();
    QIcon::setThemeName("ukui-icon-theme-default");
    MainWindow w;
    w.hide();

    return a.exec();
}
