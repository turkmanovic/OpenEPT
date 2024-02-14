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
