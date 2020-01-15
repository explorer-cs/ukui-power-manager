#ifndef DEVICE_WIDGET_H
#define DEVICE_WIDGET_H

#include <QWidget>

namespace Ui {
class DeviceWidget;
}

class DeviceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceWidget(QWidget *parent = nullptr);
    ~DeviceWidget();

    void setRemain(QString remain);
    void setState(QString state);
    void setPercent(QString perct);
    void setIcon(QString name);
    QString icon_name;
    QString state_text;
    QString percentage;
    QString predict;

public Q_SLOTS:
    void widget_property_change();
protected:
    void mouseReleaseEvent(QMouseEvent *event);
private:
    Ui::DeviceWidget *ui;
};

#endif // DEVICE_WIDGET_H
