#include "device_form.h"
#include "ui_deviceform.h"

DeviceForm::DeviceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceForm)
{
    ui->setupUi(this);
}

DeviceForm::~DeviceForm()
{
    delete ui;
}
