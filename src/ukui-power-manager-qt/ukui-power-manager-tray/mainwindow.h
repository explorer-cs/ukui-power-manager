#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGSettings>
#include <QSystemTrayIcon>
//#include "powerdispwidget.h"
#include <QMenu>
#include <QScrollArea>
#include "device_widget.h"
#include "enginedevice.h"
#include <QDBusObjectPath>
#include <QDBusMessage>
#include "customtype.h"
#include <QDBusConnection>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initUi();
    void initUi2();
    void get_power_list();
    void initData();
    int get_engine_dev_number();
public Q_SLOTS:
    void iconThemeChanged();
    void onActivatedIcon(QSystemTrayIcon::ActivationReason reason);
    void set_preference_func();
    void show_percentage_func();
    void on_icon_changed(QString str);
    void on_sum_changed(QString str);
private:
//    QGSettings *setting;
//    PowerDispWidget *dispWidget;
    QSystemTrayIcon* trayIcon;
    bool disp_control;
    QList<QDBusObjectPath> deviceNames;
    EngineDevice* ed;

    QMenu *menu;
    QAction *set_preference;
    QAction *show_percentage;
    QScrollArea *scroll_area;
    Ui::MainWindow *ui;
    QWidget *pow_widget;
    QList<DeviceWidget *> device_items;
    QString releaseQss;
    QString pressQss;
    bool saving;
    bool healthing;
protected:
    bool event(QEvent *event);
private Q_SLOTS:
    void on_savebtn_pressed();

    void on_savebtn_released();

    void on_healthbtn_pressed();

    void on_healthbtn_released();

    void on_brightbtn_pressed();

    void on_brightbtn_released();

private:

};

#endif // MAINWINDOW_H
