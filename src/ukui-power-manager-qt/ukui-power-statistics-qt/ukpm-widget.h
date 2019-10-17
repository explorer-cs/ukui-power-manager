#ifndef UKPM_WIDGET_H
#define UKPM_WIDGET_H

#include <QMap>
#include <QList>
#include <QProcess>
#include <QDebug>
#include <QTime>
#include <QDateTime>
#include <QToolButton>
#include <QWidget>
#include <QListWidget>
#include <QTabWidget>
#include <QStackedWidget>
#include <QTableWidget>
#include <QSplitter>
#include "QLabel"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QSpacerItem>
#include <QChart>
#include <QLineSeries>
#include <QChartView>
#include <QPainter>
#include <QValueAxis>
#include <QTableWidget>
#include <QHeaderView>
#include <QIcon>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QDesktopWidget>
#include <QApplication>
#include <QSplineSeries>
#include <QDesktopServices>
#include <QDBusConnection>
#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusReply>
#include <QFormLayout>
#include <QWidgetAction>
#include "gsettings.h"
#include <QCategoryAxis>

#define WORKING_DIRECTORY "."
#define DBUS_SERVICE "org.freedesktop.UPower"
#define DBUS_OBJECT "/org/freedesktop/UPower"
#define DBUS_INTERFACE "org.freedesktop.DBus.Properties"
#define DBUS_INTERFACE_PARAM "org.freedesktop.UPower.Device"

enum SUMTYPE
{
    CHARGE,CHARGE_ACCURENCY,DISCHARGING,DISCHARGING_ACCURENCY
};

enum HISTYPE
{
    RATE,VOLUME,CHARGE_DURATION,DISCHARGING_DURATION
};

enum TIMESPAN
{
    TENM,TWOH,SIXH,ONED,ONEW
};

struct DCDetail
{
    QString Device;
    QString Type;
    QString PowerSupply;
    QString Online;
};

struct DEV
{
    QString Device;
    QString Type;
    QString PowerSupply;
    QString Online;
    QString Vendor;
    QString Model;
    QString Refresh;
    QString Energy;
    QString EnergyEmpty;
    QString EnergyFull;
    QString EnergyFullDesign;
    QString EnergyRate;
    QString IsPresent;
    QString IsRechargeable;
    QString Percentage;
    QString State;
    QString TimeToEmpty;
    QString TimeToFull;
    QString Voltage;
    QString Capacity;
};
struct BTRDetail
{
    QString Device;
    QString Type;
    QString Vendor;
    QString Model;
    QString PowerSupply;
    QString Refresh;
    QString Energy;
    QString EnergyEmpty;
    QString EnergyFull;
    QString EnergyFullDesign;
    QString EnergyRate;
    QString IsPresent;
    QString IsRechargeable;
    QString Percentage;
    QString State;
    QString TimeToEmpty;
    QString TimeToFull;
    QString Voltage;
    QString Capacity;
};


QT_CHARTS_USE_NAMESPACE
class UkpmWidget : public QWidget
{
    Q_OBJECT

public:
    UkpmWidget(QWidget *parent = 0);
    ~UkpmWidget();
    void setUI();
    void setHistoryTab();
    void setSumTab();
    void connectSlots();
    QList<QPointF> setdata(); //设置图表数据的函数接口
    void getDcDetail(QString dcStr = NULL);
    void getBtrDetail();
    void getAll(BTRDetail *dc);
    void putAttributes(QMap<QString, QVariant> &map);
    void calcTime(QString &attr, qlonglong time);
    void initBtrDetail(QString btr);
    void getDevices();
    void setupDcUI();
    void setupBtrUI();
    void initUI();
    void addNewUI(QDBusObjectPath &path);
    int parseArguments();
    QString getWidgetAxis(uint value);

public Q_SLOTS:
    void updateHisChart(int);
    void updateSumChart(int);
    void sortDcTable(int id);
    void sortBtrTable(int id);
    void onActivatedIcon(QSystemTrayIcon::ActivationReason reason);
    void onShow();
    void showHisDataPoint(bool flag);
    void showSumDataPoint(bool flag);
    void onHelpButtonClicked(bool);
    void onExitButtonClicked(bool);
    void updateHisType(int index);
    void drawSumSpineline(bool flag);
    void drawHisSpineline(bool flag);
    void helpFormat();

    void onUSBDeviceHotPlug(int drvid, int action, int devNumNow);

    void btrPropertiesChanged(QDBusMessage msg);
    void callFinishedSlot(QDBusPendingCallWatcher *call);
    void control_center_power();
    void deviceAdded(QDBusMessage msg);
    void deviceRemoved(QDBusMessage msg);
    void acPropertiesChanged(QDBusMessage msg);

    void onBtrPageChanged(int index);
protected:
    void minimumSize();
public:
    uint timeSpan, resolution;
    DEV dcDetailData;
    DEV btrDetailData;
    QListWidget *listWidget;
    QTabWidget *tabWidgetDC, *tabWidgetBTR;
    QStackedWidget *stackedWidget;

    HISTYPE mHISTYPE;
    SUMTYPE mSUMTYPE;
    QChart *hisChart;
    QChartView *hisChartView;
    QLineSeries *hisSeries;
    QSplineSeries *hisSpline;
    QCategoryAxis *axisY;
    QCategoryAxis *xtime;

    QValueAxis *x, *y;
    QChart *sumChart;
    QChartView *sumChartView;
    QLineSeries *sumSeries;
    QSplineSeries *sumSpline;
    QComboBox *sumTypeCombox;
    QComboBox *spanCombox ;
    QComboBox *typeCombox;

    bool spineLineSum, spineLineHis;

    QTableWidget *detailDcTable;
    QTableWidget *detailBTRTable;
    QHeaderView *headView;
    QSystemTrayIcon *systemTrayIcon;
    QMenu *menu;
    QCheckBox *hisDataBox, *sumDataBox;
    QCheckBox *hisCurveBox, *sumCurveBox;
    QToolButton *exitButton, *helpButton;

    QDBusInterface *dbusService,*serviceInterface;
    QString addString,toolTip;
    uint flag;
    QList<QDBusObjectPath> deviceNames;
    QList<DEV> devices;
    QMap<QDBusObjectPath,QListWidgetItem*> listItem;
    QMap<QDBusObjectPath,QTabWidget*> widgetItem;
    QString batterySvr,acSvr;
    bool iconflag;
    QGSettings *settings;

};

#endif // UKPM_WIDGET_H
