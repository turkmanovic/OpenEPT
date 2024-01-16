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
    connect(device, SIGNAL(sigStatusLinkNewDeviceAdded(QString)), this, SLOT(onDeviceStatusLinkNewDeviceAdded(QString)));
    connect(device, SIGNAL(sigStatusLinkNewMessageReceived(QString,QString)), this, SLOT(onDeviceStatusLinkNewMessageReceived(QString,QString)));

    log->printLogMessage("Device container successfully created", LOG_MESSAGE_TYPE_INFO);
    device->statusLinkCreate();

}

DeviceContainer::~DeviceContainer()
{
    delete deviceWnd;
    delete device;
    delete log;
}

void DeviceContainer::onDeviceControlLinkDisconnected()
{
    log->printLogMessage("Device control link disconnected", LOG_MESSAGE_TYPE_WARNING);
    deviceWnd->setDeviceState(DEVICE_STATE_DISCONNECTED);
}

void DeviceContainer::onDeviceControlLinkConnected()
{
    log->printLogMessage("Device control link established", LOG_MESSAGE_TYPE_INFO);
    deviceWnd->setDeviceState(DEVICE_STATE_CONNECTED);
}

void DeviceContainer::onDeviceStatusLinkNewDeviceAdded(QString aDeviceIP)
{
    log->printLogMessage("Status link sucessfully establish with device(IP: " + aDeviceIP + ")", LOG_MESSAGE_TYPE_INFO);
}

void DeviceContainer::onDeviceStatusLinkNewMessageReceived(QString aDeviceIP, QString aMessage)
{
    log->printLogMessage("New message received from device (IP: " + aDeviceIP + ") :\" " + aMessage + "\"", LOG_MESSAGE_TYPE_INFO, LOG_MESSAGE_DEVICE_TYPE_DEVICE);
}

void DeviceContainer::onDeviceClosed()
{
    emit sigDeviceClosed(device);
}
