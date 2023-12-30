#include "devicecontainer.h"

DeviceContainer::DeviceContainer(QObject *parent,  DeviceWnd* aDeviceWnd, Device* aDevice)
    : QObject{parent}
{
    deviceWnd   = aDeviceWnd;
    device      = aDevice;
    log         = new Log();
    log->assignLogWidget(deviceWnd->getLogWidget());

    connect(deviceWnd, SIGNAL(sigWndClosed()), this, SLOT(onDeviceClosed()));
    connect(device, SIGNAL(sigControlLinkConnected()), this, SLOT(onDeviceControlLinkConnected()));
    connect(device, SIGNAL(sigControlLinkDisconnected()), this, SLOT(onDeviceControlLinkDisconnected()));
    log->printLogMessage("Device container successfully created", LOG_MESSAGE_TYPE_INFO);

}

void DeviceContainer::onDeviceControlLinkDisconnected()
{
    log->printLogMessage("Device control link disconnected", LOG_MESSAGE_TYPE_WARNING);
}

void DeviceContainer::onDeviceControlLinkConnected()
{
    log->printLogMessage("Device control link established", LOG_MESSAGE_TYPE_INFO);
}

void DeviceContainer::onDeviceClosed()
{
    emit sigDeviceClosed(device);
}
