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

void Device::assignControlLink(ControlLink* link)
{
    controlLink = link;
    connect(controlLink, SIGNAL(sigConnected()), this, SLOT(onControlLinkConnected()));
    connect(controlLink, SIGNAL(sigDisconnected()), this, SLOT(onControlLinkDisconnected()));
    emit sigControlLinkConnected();

}

void Device::onControlLinkConnected()
{
    emit sigControlLinkConnected();
}

void Device::onControlLinkDisconnected()
{
    emit sigControlLinkDisconnected();
}
