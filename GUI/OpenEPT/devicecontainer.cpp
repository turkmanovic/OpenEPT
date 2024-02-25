#include "devicecontainer.h"

DeviceContainer::DeviceContainer(QObject *parent,  DeviceWnd* aDeviceWnd, Device* aDevice)
    : QObject{parent}
{
    deviceWnd   = aDeviceWnd;
    device      = aDevice;
    log         = new Log();
    log->assignLogWidget(deviceWnd->getLogWidget());

    connect(deviceWnd,  SIGNAL(sigWndClosed()), this, SLOT(onDeviceClosed()));
    connect(deviceWnd,  SIGNAL(sigNewControlMessageRcvd(QString)), this, SLOT(onConsoleWndMessageRcvd(QString)));
    connect(device,     SIGNAL(sigControlLinkConnected()), this, SLOT(onDeviceControlLinkConnected()));
    connect(device,     SIGNAL(sigControlLinkDisconnected()), this, SLOT(onDeviceControlLinkDisconnected()));
    connect(device,     SIGNAL(sigStatusLinkNewDeviceAdded(QString)), this, SLOT(onDeviceStatusLinkNewDeviceAdded(QString)));
    connect(device,     SIGNAL(sigStatusLinkNewMessageReceived(QString,QString)), this, SLOT(onDeviceStatusLinkNewMessageReceived(QString,QString)));
    connect(device,     SIGNAL(sigNewResponseReceived(QString)), this, SLOT(onConsoleWndHandleControlMsgResponse(QString)));
    connect(deviceWnd,  SIGNAL(sigResolutionChanged(QString)), this, SLOT(onResolutionChanged(QString)));
    connect(deviceWnd,  SIGNAL(sigClockDivChanged(QString)), this, SLOT(onClockDivChanged(QString)));
    connect(deviceWnd,  SIGNAL(sigSampleTimeChanged(QString)), this, SLOT(onSampleTimeChanged(QString)));
    connect(deviceWnd,  SIGNAL(sigSamplingTimeChanged(QString)), this, SLOT(onSamplingTimeChanged(QString)));
    connect(deviceWnd,  SIGNAL(sigAvrRatioChanged(int)), this, SLOT(onAvrRatioChanged(int)));
    connect(deviceWnd,  SIGNAL(sigVOffsetChanged(QString)), this, SLOT(onVOffsetChanged(QString)));
    connect(deviceWnd,  SIGNAL(sigCOffsetChanged(QString)), this, SLOT(onCOffsetChanged(QString)));
    connect(deviceWnd,  SIGNAL(sigNewInterfaceSelected(QString)), this, SLOT(onInterfaceChanged(QString)));
    connect(deviceWnd,  SIGNAL(sigStartAcquisition()), this, SLOT(onAcquisitionStart()));
    connect(deviceWnd,  SIGNAL(sigStopAcquisition()), this, SLOT(onAcquisitionStop()));
    connect(deviceWnd,  SIGNAL(sigPauseAcquisition()), this, SLOT(onAcquisitionPause()));

    connect(device,     SIGNAL(sigResolutionObtained(QString)), this, SLOT(onDeviceResolutionObtained(QString)));
    connect(device,     SIGNAL(sigChSampleTimeObtained(QString)), this, SLOT(onDeviceChSampleTimeObtained(QString)));
    connect(device,     SIGNAL(sigClockDivObtained(QString)), this, SLOT(onDeviceClkDivObtained(QString)));
    connect(device,     SIGNAL(sigSampleTimeObtained(QString)), this, SLOT(onDeviceSTimeObtained(QString)));

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

void DeviceContainer::onResolutionChanged(QString resolution)
{
    /* call deviceWnd function with recieved msg from FW <- */
    device_adc_resolution_t tmpResolution = getAdcResolutionFromString(resolution);
    if(!device->setResolution(tmpResolution))
    {
        log->printLogMessage("Unable to set resolution: " + resolution, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Resolution: " + resolution + " sucessfully set", LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onClockDivChanged(QString clockDiv)
{
    /* call deviceWnd function with recieved msg from FW <- */
    device_adc_clock_div_t tmpClockDiv = getAdcClockDivFromString(clockDiv);
    if(!device->setClockDiv(tmpClockDiv))
    {
        log->printLogMessage("Unable to set clock div: " + clockDiv, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Clock div: " + clockDiv + " sucessfully set", LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onSampleTimeChanged(QString stime)
{
    /* call deviceWnd function with recieved msg from FW <- */
    device_adc_ch_sampling_time_t tmpSampleTime = getAdcChSamplingTimeFromString(stime);
    if(!device->setChSampleTime(tmpSampleTime))
    {
        log->printLogMessage("Unable to set sample time: " + stime, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Sample time: " + stime + " sucessfully set", LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onAvrRatioChanged(QString avgRatio)
{
    /* call deviceWnd function with recieved msg from FW <- */
    device_adc_averaging_t tmpAveragingRatio = getAdcAvgRatioFromString(avgRatio);
    if(!device->setAvrRatio(tmpAveragingRatio))
    {
        log->printLogMessage("Unable to set averaging ratio: " + avgRatio, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Averaging ratio: " + avgRatio + " sucessfully set", LOG_MESSAGE_TYPE_INFO);
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
        log->printLogMessage("Sampling time: " + time + " sucessfully set", LOG_MESSAGE_TYPE_INFO);
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
        log->printLogMessage("Voltage offset: " + off + " sucessfully set", LOG_MESSAGE_TYPE_INFO);
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
        log->printLogMessage("Current offset: " + off + " sucessfully set", LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onAcquisitionStart()
{
    if(!device->acquisitionStart())
    {
        log->printLogMessage("Unable to start acquistion", LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Acquisition sucessfully started", LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onAcquisitionStop()
{
    if(!device->acquisitionStop())
    {
        log->printLogMessage("Unable to stop acquistion", LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Acquisition sucessfully stoped", LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onAcquisitionPause()
{
    if(!device->acquisitionPause())
    {
        log->printLogMessage("Unable to pause acquistion", LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Acquisition sucessfully paused", LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onDeviceResolutionObtained(QString resolution)
{
    if(!deviceWnd->setResolution(resolution))
    {
        log->printLogMessage("Unable to show obtained resolution: " + resolution + "bit", LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Resolution sucessfully obtained and presented ", LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onDeviceClkDivObtained(QString clkDiv)
{
    if(!deviceWnd->setClkDiv(clkDiv))
    {
        log->printLogMessage("Unable to show obtained clock div: " + clkDiv, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Clock div sucessfully obained and presented ", LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onDeviceChSampleTimeObtained(QString stime)
{
    if(!deviceWnd->setChSamplingTime(stime))
    {
        log->printLogMessage("Unable to show obtained channels sampling time: " + stime, LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Channels sampling time sucessfully obained and presented ", LOG_MESSAGE_TYPE_INFO);
    }
}

void DeviceContainer::onDeviceSTimeObtained(QString stime)
{
    if(!deviceWnd->setSTime(stime))
    {
        log->printLogMessage("Unable to show obtained sampling time: " + stime + "us", LOG_MESSAGE_TYPE_ERROR);
    }
    else
    {
        log->printLogMessage("Sampling time sucessfully obained and presented ", LOG_MESSAGE_TYPE_INFO);
    }
}

device_adc_resolution_t DeviceContainer::getAdcResolutionFromString(QString resolution)
{
    device_adc_resolution_t returnResolution = DEVICE_ADC_RESOLUTION_UKNOWN;
    QStringList *resolutionOptions = deviceWnd->getResolutionOptions();
    switch(resolutionOptions->indexOf(resolution))
    {
    case 0:
        returnResolution = DEVICE_ADC_RESOLUTION_UKNOWN;
        break;
    case 1:
        returnResolution = DEVICE_ADC_RESOLUTION_16BIT;
        break;
    case 2:
        returnResolution = DEVICE_ADC_RESOLUTION_14BIT;
        break;
    case 3:
        returnResolution = DEVICE_ADC_RESOLUTION_12BIT;
        break;
    case 4:
        returnResolution = DEVICE_ADC_RESOLUTION_10BIT;
        break;
    default:
        returnResolution = DEVICE_ADC_RESOLUTION_UKNOWN;
        break;
    }
    return returnResolution;
}

device_adc_clock_div_t DeviceContainer::getAdcClockDivFromString(QString clkDiv)
{
    device_adc_clock_div_t returnClkDiv = DEVICE_ADC_CLOCK_DIV_UKNOWN;
    QStringList *clkDivOptions = deviceWnd->getClockDivOptions();
    switch(clkDivOptions->indexOf(clkDiv))
    {
    case 0:
        returnClkDiv = DEVICE_ADC_CLOCK_DIV_UKNOWN;
        break;
    case 1:
        returnClkDiv = DEVICE_ADC_CLOCK_DIV_1;
        break;
    case 2:
        returnClkDiv = DEVICE_ADC_CLOCK_DIV_2;
        break;
    case 3:
        returnClkDiv = DEVICE_ADC_CLOCK_DIV_4;
        break;
    case 4:
        returnClkDiv = DEVICE_ADC_CLOCK_DIV_8;
        break;
    case 5:
        returnClkDiv = DEVICE_ADC_CLOCK_DIV_16;
        break;
    case 6:
        returnClkDiv = DEVICE_ADC_CLOCK_DIV_32;
        break;
    case 7:
        returnClkDiv = DEVICE_ADC_CLOCK_DIV_64;
        break;
    case 9:
        returnClkDiv = DEVICE_ADC_CLOCK_DIV_128;
        break;
    default:
        returnClkDiv = DEVICE_ADC_CLOCK_DIV_UKNOWN;
        break;
    }
    return returnClkDiv;
}

device_adc_ch_sampling_time_t DeviceContainer::getAdcChSamplingTimeFromString(QString chstime)
{
    device_adc_ch_sampling_time_t returnSTime = DEVICE_ADC_SAMPLING_TIME_UKNOWN;
    QStringList *stimeOptions = deviceWnd->getChSamplingTimeOptions();
    switch(stimeOptions->indexOf(chstime))
    {
    case 0:
        returnSTime = DEVICE_ADC_SAMPLING_TIME_UKNOWN;
        break;
    case 1:
        returnSTime = DEVICE_ADC_SAMPLING_TIME_1C5;
        break;
    case 2:
        returnSTime = DEVICE_ADC_SAMPLING_TIME_2C5;
        break;
    case 3:
        returnSTime = DEVICE_ADC_SAMPLING_TIME_8C5;
        break;
    case 4:
        returnSTime = DEVICE_ADC_SAMPLING_TIME_16C5;
        break;
    case 5:
        returnSTime = DEVICE_ADC_SAMPLING_TIME_32C5;
        break;
    case 6:
        returnSTime = DEVICE_ADC_SAMPLING_TIME_64C5;
        break;
    case 7:
        returnSTime = DEVICE_ADC_SAMPLING_TIME_387C5;
        break;
    case 9:
        returnSTime = DEVICE_ADC_SAMPLING_TIME_810C5;
        break;
    default:
        returnSTime = DEVICE_ADC_SAMPLING_TIME_UKNOWN;
        break;
    }
    return returnSTime;
}

device_adc_averaging_t DeviceContainer::getAdcAvgRatioFromString(QString avgRatio)
{
    device_adc_averaging_t returnAvgRatio = DEVICE_ADC_AVERAGING_UKNOWN;
    QStringList *avgOptions = deviceWnd->getChAvgRationOptions();
    switch(avgOptions->indexOf(avgRatio))
    {
    case 0:
        returnAvgRatio = DEVICE_ADC_AVERAGING_UKNOWN;
        break;
    case 1:
        returnAvgRatio = DEVICE_ADC_AVERAGING_DISABLED;
        break;
    case 2:
        returnAvgRatio = DEVICE_ADC_AVERAGING_2;
        break;
    case 3:
        returnAvgRatio = DEVICE_ADC_AVERAGING_4;
        break;
    case 4:
        returnAvgRatio = DEVICE_ADC_AVERAGING_8;
        break;
    case 5:
        returnAvgRatio = DEVICE_ADC_AVERAGING_16;
        break;
    case 6:
        returnAvgRatio = DEVICE_ADC_AVERAGING_32;
        break;
    case 7:
        returnAvgRatio = DEVICE_ADC_AVERAGING_64;
        break;
    case 9:
        returnAvgRatio = DEVICE_ADC_AVERAGING_128;
        break;
    case 10:
        returnAvgRatio = DEVICE_ADC_AVERAGING_256;
        break;
    case 11:
        returnAvgRatio = DEVICE_ADC_AVERAGING_512;
        break;
    case 12:
        returnAvgRatio = DEVICE_ADC_AVERAGING_1024;
        break;
    default:
        returnAvgRatio = DEVICE_ADC_AVERAGING_UKNOWN;
        break;
    }
    return returnAvgRatio;
}
