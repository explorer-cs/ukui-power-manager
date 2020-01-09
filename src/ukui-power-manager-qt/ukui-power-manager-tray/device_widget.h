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
protected:
    void mouseReleaseEvent(QMouseEvent *event);
private:
    Ui::DeviceWidget *ui;
};

#endif // DEVICE_WIDGET_H
