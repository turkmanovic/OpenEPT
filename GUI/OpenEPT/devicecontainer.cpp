#include "devicecontainer.h"

DeviceContainer::DeviceContainer(QObject *parent,  DeviceWnd* aDeviceWnd, Device* aDevice)
    : QObject{parent}
{
    deviceWnd   = aDeviceWnd;
    device      = aDevice;

    connect(deviceWnd, SIGNAL(sigWndClosed()), this, SLOT(onDeviceClosed()));


}

void DeviceContainer::onDeviceClosed()
{
    emit sigDeviceClosed(device);
}
