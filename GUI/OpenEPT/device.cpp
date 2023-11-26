#include "device.h"

Device::Device(QObject *parent)
    : QObject{parent}
{
    adcResolution    = DEVICE_ADC_RESOLUTION_UKNOWN;
    adcSamplingSpeed = DEVICE_ADC_SAMPLING_SPEED_UKNOWN;
    adcAveraging     = DEVICE_ADC_AVERAGING_UKNOWN;
    adcClockingDiv   = DEVICE_ADC_CLOCK_DIV_UKNOWN;
    deviceName       = "";
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
