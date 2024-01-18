#include "adddevicewnd.h"
#include "ui_adddevicewnd.h"
#include "ui_consolewnd.h"

AddDeviceWnd::AddDeviceWnd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddDeviceWnd)
{
    ui->setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    connect(ui->closePusb, SIGNAL(pressed()), this, SLOT(onClosePusbPressed()));
    connect(ui->addDevicePusb, SIGNAL(pressed()), this, SLOT(onAddDevicePusbPressed()));
}

AddDeviceWnd::~AddDeviceWnd()
{
    delete ui;
}

void AddDeviceWnd::onClosePusbPressed()
{
    close();
}

void AddDeviceWnd::onAddDevicePusbPressed()
{
    emit sigAddDevice(ui->ipAddressLine->text(), ui->portNumberLine->text());
    close();
}
