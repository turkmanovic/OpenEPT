#include "device.h"

Device::Device(QObject *parent)
    : QObject{parent}
{
    adcResolution    = DEVICE_ADC_RESOLUTION_UKNOWN;
    adcSamplingTime = DEVICE_ADC_SAMPLING_TIME_UKNOWN;
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
