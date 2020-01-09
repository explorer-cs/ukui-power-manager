#include "device_widget.h"
#include "ui_devicewidget.h"

DeviceWidget::DeviceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceWidget)
{
    ui->setupUi(this);
}

DeviceWidget::~DeviceWidget()
{
    delete ui;
}

void DeviceWidget::setIcon(QString name)
{
    QPixmap pix(name);
    ui->devicon->setPixmap(pix);
}

void DeviceWidget::setPercent(QString perct)
{
    ui->percentage->setText(perct);
}

void DeviceWidget::setState(QString state)
{
    ui->state->setText(state);
}

void DeviceWidget::setRemain(QString remain)
{
    ui->remaintext->setText(tr("RemainTime"));
    ui->remaindata->setText(remain);
}

void DeviceWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QString cmd = "ukui-power-statistics-qt &";
    system(cmd.toStdString().c_str());
}
