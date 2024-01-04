#include "device.h"

Device::Device(QObject *parent)
    : QObject{parent}
{
    adcResolution       = DEVICE_ADC_RESOLUTION_UKNOWN;
    adcSamplingTime     = DEVICE_ADC_SAMPLING_TIME_UKNOWN;
    adcAveraging        = DEVICE_ADC_AVERAGING_UKNOWN;
    adcClockingDiv      = DEVICE_ADC_CLOCK_DIV_UKNOWN;
    deviceName          = "";
    controlLink         = NULL;
}

Device::~Device()
{
    delete controlLink;
}

bool Device::setName(QString aNewDeviceName)
{
    deviceName  = aNewDeviceName;
    return true;
}

bool Device::getName(QString *aDeviceName)
{
    *aDeviceName = deviceName;
    return true;
}

void Device::controlLinkAssign(ControlLink* link)
{
    controlLink = link;
    connect(controlLink, SIGNAL(sigConnected()), this, SLOT(onControlLinkConnected()));
    connect(controlLink, SIGNAL(sigDisconnected()), this, SLOT(onControlLinkDisconnected()));
    emit sigControlLinkConnected();

}

void Device::statusLinkCreate()
{
    statusLink  = new StatusLink();
    statusLink->startServer();
    connect(statusLink, SIGNAL(sigNewClientConnected(QString)), this, SLOT(onStatusLinkNewDeviceAdded(QString)));
    connect(statusLink, SIGNAL(sigNewStatusMessageReceived(QString,QString)), this, SLOT(onStatusLinkNewMessageReceived(QString,QString)));
}

void Device::controlLinkReconnect()
{
    controlLink->reconnect();
}

void Device::onControlLinkConnected()
{
    emit sigControlLinkConnected();
}

void Device::onControlLinkDisconnected()
{
    emit sigControlLinkDisconnected();
}

void Device::onStatusLinkNewDeviceAdded(QString aDeviceIP)
{
    emit sigStatusLinkNewDeviceAdded(aDeviceIP);
}

void Device::onStatusLinkNewMessageReceived(QString aDeviceIP, QString aMessage)
{
    emit sigStatusLinkNewMessageReceived(aDeviceIP, aMessage);
}
