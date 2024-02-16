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

void Device::sendControlMsg(QString msg)
{
    /* call controLink execute Commnad to communicate with FW -> */
    QString response;
    if(!controlLink->executeCommand(msg, &response, 1000))return;
    /* emit Response to deviceContainer <- */
    emit sigNewResponseReceived(response);
}

bool Device::setResolution(device_adc_resolution_t resolution)
{
    QString response;
    QString command = "device adc chresolution set -value=";
    switch(resolution)
    {
    case DEVICE_ADC_RESOLUTION_UKNOWN:
        return false;
        break;
    case DEVICE_ADC_RESOLUTION_16BIT:
        command += "16";
        break;
    case DEVICE_ADC_RESOLUTION_14BIT:
        command += "14";
        break;
    case DEVICE_ADC_RESOLUTION_12BIT:
        command += "12";
        break;
    case DEVICE_ADC_RESOLUTION_10BIT:
        command += "10";
        break;
    }
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    //Proveriti da li je ok, ako nije vratiti false, ako jeste vratiti true
    if(response != "OK"){
        return false;
    }
    adcResolution = resolution;
    return true;
}

bool Device::setClockDiv(device_adc_clock_div_t clockDiv)
{
    QString response;
    QString command = "device adc chclkdiv set -value=";
    switch(clockDiv)
    {
    case DEVICE_ADC_CLOCK_DIV_UKNOWN:
        return false;
        break;
    case DEVICE_ADC_CLOCK_DIV_1:
        command += "1";
        break;
    case DEVICE_ADC_CLOCK_DIV_2:
        command += "2";
        break;
    case DEVICE_ADC_CLOCK_DIV_4:
        command += "4";
        break;
    case DEVICE_ADC_CLOCK_DIV_6:
        command += "6";
        break;
    case DEVICE_ADC_CLOCK_DIV_8:
        command += "8";
        break;
    case DEVICE_ADC_CLOCK_DIV_10:
        command += "10";
        break;
    case DEVICE_ADC_CLOCK_DIV_12:
        command += "12";
        break;
    case DEVICE_ADC_CLOCK_DIV_16:
        command += "16";
        break;
    case DEVICE_ADC_CLOCK_DIV_32:
        command += "32";
        break;
    case DEVICE_ADC_CLOCK_DIV_64:
        command += "64";
        break;
    case DEVICE_ADC_CLOCK_DIV_128:
        command += "128";
        break;
    }
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    if(response != "OK"){
        return false;
    }
    adcClockingDiv = clockDiv;
    return true;
}

bool Device::setSampleTime(device_adc_sampling_time_t sampleTime)
{
    QString response;
    QString command = "device adc chstime set -value=";
    switch(sampleTime)
    {
    case DEVICE_ADC_SAMPLING_TIME_UKNOWN:
        return false;
        break;
    case DEVICE_ADC_SAMPLING_TIME_1C5:
        command += "1";
        break;
    case DEVICE_ADC_SAMPLING_TIME_2C5:
        command += "2";
        break;
    case DEVICE_ADC_SAMPLING_TIME_8C5:
        command += "8";
        break;
    case DEVICE_ADC_SAMPLING_TIME_16C5:
        command += "16";
        break;
    case DEVICE_ADC_SAMPLING_TIME_32C5:
        command += "32";
        break;
    case DEVICE_ADC_SAMPLING_TIME_64C5:
        command += "64";
        break;
    case DEVICE_ADC_SAMPLING_TIME_387C5:
        command += "387";
        break;
    case DEVICE_ADC_SAMPLING_TIME_810C5:
        command += "810";
        break;
    }
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    if(response != "OK"){
        return false;
    }
    adcSamplingTime = sampleTime;
    return true;
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
