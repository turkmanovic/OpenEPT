#include "devicewnd.h"
#include "ui_devicewnd.h"

DeviceWnd::DeviceWnd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceWnd)
{
    ui->setupUi(this);
}

DeviceWnd::~DeviceWnd()
{
    delete ui;
}
