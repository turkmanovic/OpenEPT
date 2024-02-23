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
    connect(deviceWnd, SIGNAL(sigSamplingTimeChanged(QString)), this, SLOT(onSamplingTimeChanged(QString)));
    connect(deviceWnd, SIGNAL(sigAvrRatioChanged(int)), this, SLOT(onAvrRatioChanged(int)));
    connect(deviceWnd, SIGNAL(sigVOffsetChanged(QString)), this, SLOT(onVOffsetChanged(QString)));
    connect(deviceWnd, SIGNAL(sigCOffsetChanged(QString)), this, SLOT(onCOffsetChanged(QString)));
    connect(deviceWnd, SIGNAL(sigNewInterfaceSelected(QString)), this, SLOT(onInterfaceChanged(QString)));

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
        deviceWnd->printConsoleMsg("Unable to set clock div");
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
    device_adc_ch_sampling_time_t tmpSampleTime;
    QString tmpSampleTimeString = "";
    switch(index){
    default:
        deviceWnd->printConsoleMsg("Unable to set sample time");
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
    if(!device->setChSampleTime(tmpSampleTime))
    {
        log->printLogMessage("Unable to set sample time: " + tmpSampleTimeString, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Sample time: " + tmpSampleTimeString, LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onAvrRatioChanged(int index)
{
    /* call deviceWnd function with recieved msg from FW <- */
    device_adc_averaging_t tmpAveragingRatio;
    QString tmpAveragingRatioString = "";
    switch(index){
    default:
        deviceWnd->printConsoleMsg("Unable to set averaging ratio");
        return;
    case 0:
        tmpAveragingRatio = DEVICE_ADC_AVERAGING_UKNOWN;
        tmpAveragingRatioString = "Uknown";
        break;
    case 1:
        tmpAveragingRatio = DEVICE_ADC_AVERAGING_DISABLED;
        tmpAveragingRatioString = "1";
        break;
    case 2:
        tmpAveragingRatio = DEVICE_ADC_AVERAGING_2;
        tmpAveragingRatioString = "2";
        break;
    case 3:
        tmpAveragingRatio = DEVICE_ADC_AVERAGING_4;
        tmpAveragingRatioString = "4";
        break;
    case 4:
        tmpAveragingRatio = DEVICE_ADC_AVERAGING_8;
        tmpAveragingRatioString = "8";
        break;
    case 5:
        tmpAveragingRatio = DEVICE_ADC_AVERAGING_16;
        tmpAveragingRatioString = "16";
        break;
    case 6:
        tmpAveragingRatio = DEVICE_ADC_AVERAGING_32;
        tmpAveragingRatioString = "32";
        break;
    case 7:
        tmpAveragingRatio = DEVICE_ADC_AVERAGING_64;
        tmpAveragingRatioString = "64";
        break;
    case 8:
        tmpAveragingRatio = DEVICE_ADC_AVERAGING_128;
        tmpAveragingRatioString = "128";
        break;
    case 9:
        tmpAveragingRatio = DEVICE_ADC_AVERAGING_256;
        tmpAveragingRatioString = "256";
        break;
    case 10:
        tmpAveragingRatio = DEVICE_ADC_AVERAGING_512;
        tmpAveragingRatioString = "512";
        break;
    case 11:
        tmpAveragingRatio = DEVICE_ADC_AVERAGING_1024;
        tmpAveragingRatioString = "1024";
        break;
    }
    if(!device->setAvrRatio(tmpAveragingRatio))
    {
        log->printLogMessage("Unable to set averaging ratio: " + tmpAveragingRatioString, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Averaging ratio: " + tmpAveragingRatioString, LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onSamplingTimeChanged(QString time)
{
    bool conversionOk;
    int  numericValue = time.toInt(&conversionOk);
    if(!(conversionOk && numericValue >0))
    {
        return;
    }

    if(!device->setSamplingTime(time))
    {
        log->printLogMessage("Unable to set sampling time: " + time, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Sampling time: " + time, LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onInterfaceChanged(QString interfaceIp)
{
    if(!device->createStreamLink(interfaceIp, 11223))
    {
        log->printLogMessage("Unable to create stream link: ", LOG_MESSAGE_TYPE_ERROR);
        deviceWnd->setDeviceInterfaceSelectionState(DEVICE_INTERFACE_SELECTION_STATE_UNDEFINED);
    }
    else
    {
        log->printLogMessage("Stream link sucessfully created: ", LOG_MESSAGE_TYPE_INFO);
        deviceWnd->setDeviceInterfaceSelectionState(DEVICE_INTERFACE_SELECTION_STATE_SELECTED);
        device->acquireDeviceConfiguration();
    }
}

void DeviceContainer::onVOffsetChanged(QString off)
{
    bool conversionOk;
    int  numericValue = off.toInt(&conversionOk);
    if(!(conversionOk && numericValue >0))
    {
        return;
    }

    if(!device->setVOffset(off))
    {
        log->printLogMessage("Unable to set voltage offset: " + off, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Voltage offset: " + off, LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onCOffsetChanged(QString off)
{
    bool conversionOk;
    int  numericValue = off.toInt(&conversionOk);
    if(!(conversionOk && numericValue >0))
    {
        return;
    }

    if(!device->setCOffset(off))
    {
        log->printLogMessage("Unable to set current offset: " + off, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Current offset: " + off, LOG_MESSAGE_TYPE_INFO);
    }
}
