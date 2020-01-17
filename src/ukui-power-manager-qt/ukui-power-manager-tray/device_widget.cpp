#include "device_widget.h"
#include "ui_devicewidget.h"
#include <QDebug>
#include <QStyleOption>
#include <QPainter>

DeviceWidget::DeviceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceWidget)
{
    ui->setupUi(this);
//    ui->DeviceWi->setStyleSheet("#wbg{background-color:#3593b5;}");
    setAttribute(Qt::WA_StyledBackground,true);
}

DeviceWidget::~DeviceWidget()
{
    delete ui;
}

void DeviceWidget::setIcon(QString name)
{
    QIcon icon = QIcon::fromTheme(name);
    qDebug()<<icon.name()<<"--------------------------------";
//    QPixmap pix(icon.name());
//    ui->devicon->setPixmap(pix);
    ui->btnicon->setIcon(icon);
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
    Q_UNUSED(event);

    QString cmd = "ukui-power-statistics-qt &";
    system(cmd.toStdString().c_str());
//    setStyleSheet(
//                "background:rgba(61,107,229,1);"
//                "border-radius:2px;"
//    );
}

void DeviceWidget::widget_property_change()
{
    setIcon(icon_name);
    setPercent(percentage);
    setState(state_text);
    setRemain(predict);
}

void DeviceWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

 void DeviceWidget::enterEvent(QEvent *event)
 {
     setStyleSheet(
                 "background:rgba(61,107,229,1);"
                 "border-radius:2px;"
     );
 }

 void DeviceWidget::leaveEvent(QEvent *event)
 {

     setStyleSheet(
                 "background:rgba(14,19,22,0.75);"
     );
 }
