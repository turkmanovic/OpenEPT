#include "devicecontainer.h"

DeviceContainer::DeviceContainer(QObject *parent,  DeviceWnd* aDeviceWnd, Device* aDevice)
    : QObject{parent}
{
    deviceWnd   = aDeviceWnd;
    device      = aDevice;
    log         = new Log();
    log->assignLogWidget(deviceWnd->getLogWidget());

    connect(deviceWnd, SIGNAL(sigWndClosed()), this, SLOT(onDeviceClosed()));
    connect(deviceWnd, SIGNAL(sigNewControlMessageRcvd(QString)), this, SLOT(onConsoleWndMessageRcvd(QString)));
    connect(device, SIGNAL(sigControlLinkConnected()), this, SLOT(onDeviceControlLinkConnected()));
    connect(device, SIGNAL(sigControlLinkDisconnected()), this, SLOT(onDeviceControlLinkDisconnected()));
    connect(device, SIGNAL(sigStatusLinkNewDeviceAdded(QString)), this, SLOT(onDeviceStatusLinkNewDeviceAdded(QString)));
    connect(device, SIGNAL(sigStatusLinkNewMessageReceived(QString,QString)), this, SLOT(onDeviceStatusLinkNewMessageReceived(QString,QString)));
    connect(device, SIGNAL(sigNewResponseReceived(QString)), this, SLOT(onConsoleWndHandleControlMsgResponse(QString)));
    connect(deviceWnd, SIGNAL(sigResolutionChanged(int)), this, SLOT(onResolutionChanged(int)));
    connect(deviceWnd, SIGNAL(sigClockDivChanged(int)), this, SLOT(onClockDivChanged(int)));
    connect(deviceWnd, SIGNAL(sigSampleTimeChanged(int)), this, SLOT(onSampleTimeChanged(int)));

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

void DeviceContainer::onConsoleWndMessageRcvd(QString msg)
{
    /* call device funtion sendControl Msg -> */
    device->sendControlMsg(msg);
}

void DeviceContainer::onConsoleWndHandleControlMsgResponse(QString msg)
{
    /* call deviceWnd function with recieved msg from FW <- */
    deviceWnd->printConsoleMsg(msg);
}

void DeviceContainer::onResolutionChanged(int index)
{
    /* call deviceWnd function with recieved msg from FW <- */
    device_adc_resolution_t tmpResolution;
    QString tmpResolutionString = "";
    switch(index){
    default:
        deviceWnd->printConsoleMsg("Unable to set resolution");
        return;
    case 0:
        tmpResolution = DEVICE_ADC_RESOLUTION_UKNOWN;
        tmpResolutionString = "Uknown";
        break;
    case 1:
        tmpResolution = DEVICE_ADC_RESOLUTION_16BIT;
        tmpResolutionString = "16 Bit";
        break;
    case 2:
        tmpResolution = DEVICE_ADC_RESOLUTION_14BIT;
        tmpResolutionString = "14 Bit";
        break;
    case 3:
        tmpResolution = DEVICE_ADC_RESOLUTION_12BIT;
        tmpResolutionString = "12 Bit";
        break;
    case 4:
        tmpResolution = DEVICE_ADC_RESOLUTION_10BIT;
        tmpResolutionString = "10 bit";
        break;
    }
    if(!device->setResolution(tmpResolution))
    {
        log->printLogMessage("Unable to set resolution: " + tmpResolutionString, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Resolution: " + tmpResolutionString, LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onClockDivChanged(int index)
{
    /* call deviceWnd function with recieved msg from FW <- */
    device_adc_clock_div_t tmpClockDiv;
    QString tmpClockDivString = "";
    switch(index){
    default:
        deviceWnd->printConsoleMsg("Unable to set resolution");
        return;
    case 0:
        tmpClockDiv = DEVICE_ADC_CLOCK_DIV_UKNOWN;
        tmpClockDivString = "Uknown";
        break;
    case 1:
        tmpClockDiv = DEVICE_ADC_CLOCK_DIV_1;
        tmpClockDivString = "1";
        break;
    case 2:
        tmpClockDiv = DEVICE_ADC_CLOCK_DIV_2;
        tmpClockDivString = "2";
        break;
    case 3:
        tmpClockDiv = DEVICE_ADC_CLOCK_DIV_4;
        tmpClockDivString = "4";
        break;
    case 4:
        tmpClockDiv = DEVICE_ADC_CLOCK_DIV_6;
        tmpClockDivString = "6";
        break;
    case 5:
        tmpClockDiv = DEVICE_ADC_CLOCK_DIV_8;
        tmpClockDivString = "8";
        break;
    case 6:
        tmpClockDiv = DEVICE_ADC_CLOCK_DIV_10;
        tmpClockDivString = "10";
        break;
    case 7:
        tmpClockDiv = DEVICE_ADC_CLOCK_DIV_12;
        tmpClockDivString = "12";
        break;
    case 8:
        tmpClockDiv = DEVICE_ADC_CLOCK_DIV_16;
        tmpClockDivString = "16";
        break;
    case 9:
        tmpClockDiv = DEVICE_ADC_CLOCK_DIV_32;
        tmpClockDivString = "32";
        break;
    case 10:
        tmpClockDiv = DEVICE_ADC_CLOCK_DIV_64;
        tmpClockDivString = "64";
        break;
    case 11:
        tmpClockDiv = DEVICE_ADC_CLOCK_DIV_128;
        tmpClockDivString = "128";
        break;
    }
    if(!device->setClockDiv(tmpClockDiv))
    {
        log->printLogMessage("Unable to set clock div: " + tmpClockDivString, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Clock div: " + tmpClockDivString, LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onSampleTimeChanged(int index)
{
    /* call deviceWnd function with recieved msg from FW <- */
    device_adc_sampling_time_t tmpSampleTime;
    QString tmpSampleTimeString = "";
    switch(index){
    default:
        deviceWnd->printConsoleMsg("Unable to set resolution");
        return;
    case 0:
        tmpSampleTime = DEVICE_ADC_SAMPLING_TIME_UKNOWN;
        tmpSampleTimeString = "Uknown";
        break;
    case 1:
        tmpSampleTime = DEVICE_ADC_SAMPLING_TIME_1C5;
        tmpSampleTimeString = "1";
        break;
    case 2:
        tmpSampleTime = DEVICE_ADC_SAMPLING_TIME_2C5;
        tmpSampleTimeString = "2";
        break;
    case 3:
        tmpSampleTime = DEVICE_ADC_SAMPLING_TIME_8C5;
        tmpSampleTimeString = "8";
        break;
    case 4:
        tmpSampleTime = DEVICE_ADC_SAMPLING_TIME_16C5;
        tmpSampleTimeString = "16";
        break;
    case 5:
        tmpSampleTime = DEVICE_ADC_SAMPLING_TIME_32C5;
        tmpSampleTimeString = "32";
        break;
    case 6:
        tmpSampleTime = DEVICE_ADC_SAMPLING_TIME_64C5;
        tmpSampleTimeString = "64";
        break;
    case 7:
        tmpSampleTime = DEVICE_ADC_SAMPLING_TIME_387C5;
        tmpSampleTimeString = "387";
        break;
    case 8:
        tmpSampleTime = DEVICE_ADC_SAMPLING_TIME_810C5;
        tmpSampleTimeString = "810";
        break;
    }
    if(!device->setSampleTime(tmpSampleTime))
    {
        log->printLogMessage("Unable to set sample time: " + tmpSampleTimeString, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Sample time: " + tmpSampleTimeString, LOG_MESSAGE_TYPE_INFO);
    }
}
