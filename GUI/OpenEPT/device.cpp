#include "stdio.h"
#include "device.h"

Device::Device(QObject *parent)
    : QObject{parent}
{
    adcResolution           = DEVICE_ADC_RESOLUTION_UKNOWN;
    adcChSamplingTime       = DEVICE_ADC_SAMPLING_TIME_UKNOWN;
    adcAveraging            = DEVICE_ADC_AVERAGING_UKNOWN;
    adcClockingDiv          = DEVICE_ADC_CLOCK_DIV_UKNOWN;
    deviceName              = "";
    samplingTime            = "";
    controlLink             = NULL;
}

Device::~Device()
{
    delete controlLink;
}

bool Device::acquisitionStart()
{
    QString response;
    QString command = "device stream start -sid=" + QString::number(streamID);
    if(controlLink == NULL) return false;
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    return true;
}

bool Device::acquisitionStop()
{
    QString response;
    QString command = "device stream stop -sid=" + QString::number(streamID);
    if(controlLink == NULL) return false;
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    return true;
}

bool Device::acquisitionPause()
{
    QString response;
    QString command = "device stream stop -sid=" + QString::number(streamID);
    if(controlLink == NULL) return false;
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    return true;
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

bool Device::createStreamLink(QString ip, quint16 port, int* id)
{
    QString response;
    QString command = "device stream create -ip=" + ip +  " -port=" + QString::number(port);
    if(controlLink == NULL) return false;
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    streamID = response.toInt();
    *id = streamID;
    return true;
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
    QString command = "device adc chresolution set -sid=" + QString::number(streamID) + " -value=";
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

bool Device::getResolution(device_adc_resolution_t *resolution)
{
    QString response;
    QString command = "device adc chresolution get -sid=" + QString::number(streamID);
    int tmpResolution;
    QString signalResponse =  "";
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    //Parse response
    tmpResolution = response.toInt();
    switch(tmpResolution)
    {
    case 16:
        adcResolution = DEVICE_ADC_RESOLUTION_16BIT;
        signalResponse += "16";
        break;
    case 14:
        adcResolution = DEVICE_ADC_RESOLUTION_14BIT;
        signalResponse += "14";
        break;
    case 12:
        adcResolution = DEVICE_ADC_RESOLUTION_12BIT;
        signalResponse += "12";
        break;
    case 10:
        adcResolution = DEVICE_ADC_RESOLUTION_10BIT;
        signalResponse += "10";
        break;
    default:
        adcResolution = DEVICE_ADC_RESOLUTION_UKNOWN;
        signalResponse += "0";
        break;
    }
    if(resolution != NULL)
    {
        *resolution = adcResolution;
    }
    emit sigResolutionObtained(signalResponse);
    return true;
}

bool Device::setClockDiv(device_adc_clock_div_t clockDiv)
{
    QString response;
    QString command = "device adc chclkdiv set -sid=" + QString::number(streamID) + " -value=";
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

bool Device::getClockDiv(device_adc_clock_div_t *clockDiv)
{
    QString response;
    QString command = "device adc chclkdiv get -sid=" + QString::number(streamID);
    int tmpClkDiv;
    QString signalResponse =  "";
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    //Parse response
    tmpClkDiv = response.toInt();
    switch(tmpClkDiv)
    {
    case 1:
        adcClockingDiv = DEVICE_ADC_CLOCK_DIV_1;
        signalResponse = "1";
        break;
    case 2:
        adcClockingDiv = DEVICE_ADC_CLOCK_DIV_2;
        signalResponse = "2";
        break;
    case 4:
        adcClockingDiv = DEVICE_ADC_CLOCK_DIV_4;
        signalResponse = "4";
        break;
    case 8:
        adcClockingDiv = DEVICE_ADC_CLOCK_DIV_8;
        signalResponse = "8";
        break;
    case 16:
        adcClockingDiv = DEVICE_ADC_CLOCK_DIV_16;
        signalResponse = "16";
        break;
    case 32:
        adcClockingDiv = DEVICE_ADC_CLOCK_DIV_32;
        signalResponse = "32";
        break;
    case 64:
        adcClockingDiv = DEVICE_ADC_CLOCK_DIV_64;
        signalResponse = "64";
        break;
    case 128:
        adcClockingDiv = DEVICE_ADC_CLOCK_DIV_128;
        signalResponse = "128";
        break;
    default:
        adcClockingDiv = DEVICE_ADC_CLOCK_DIV_UKNOWN;
        signalResponse = "0";
        break;
    }
    if(clockDiv != NULL) *clockDiv = adcClockingDiv;
    emit sigClockDivObtained(signalResponse);
    return true;
}

bool Device::setChSampleTime(device_adc_ch_sampling_time_t sampleTime)
{
    QString response;
    QString command = "device adc chstime set -sid=" + QString::number(streamID) + " -value=";
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
    adcChSamplingTime = sampleTime;
    return true;
}

bool Device::getChSampleTime(device_adc_ch_sampling_time_t *sampleTime)
{
    QString response;
    QString command = "device adc chstime get -sid=" + QString::number(streamID);
    int tmpChSTime;
    QString signalResponse =  "";
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    //Parse response
    tmpChSTime = response.toInt();
    switch(tmpChSTime)
    {
    case 1:
        adcChSamplingTime = DEVICE_ADC_SAMPLING_TIME_1C5;
        signalResponse      =   "1C5";
        break;
    case 2:
        adcChSamplingTime = DEVICE_ADC_SAMPLING_TIME_2C5;
        signalResponse      =   "2C5";
        break;
    case 8:
        adcChSamplingTime = DEVICE_ADC_SAMPLING_TIME_8C5;
        signalResponse      =   "8C5";
        break;
    case 16:
        adcChSamplingTime = DEVICE_ADC_SAMPLING_TIME_16C5;
        signalResponse      =   "16C5";
        break;
    case 32:
        adcChSamplingTime = DEVICE_ADC_SAMPLING_TIME_32C5;
        signalResponse      =   "32C5";
        break;
    case 64:
        adcChSamplingTime = DEVICE_ADC_SAMPLING_TIME_64C5;
        signalResponse      =   "64C5";
        break;
    case 128:
        adcChSamplingTime = DEVICE_ADC_SAMPLING_TIME_387C5;
        signalResponse      =   "387C5";
        break;
    default:
        adcChSamplingTime = DEVICE_ADC_SAMPLING_TIME_810C5;
        signalResponse      =   "810C5";
        break;
    }
    if(sampleTime != NULL) *sampleTime = adcChSamplingTime;
    emit sigChSampleTimeObtained(signalResponse);
    return true;
}

bool Device::setAvrRatio(device_adc_averaging_t averagingRatio)
{
    QString response;
    QString command = "device adc chavrratio set -sid=" + QString::number(streamID) + " -value=";
    switch(averagingRatio)
    {
    case DEVICE_ADC_AVERAGING_UKNOWN:
        return false;
        break;
    case DEVICE_ADC_AVERAGING_DISABLED:
        command += "1";
        break;
    case DEVICE_ADC_AVERAGING_2:
        command += "2";
        break;
    case DEVICE_ADC_AVERAGING_4:
        command += "4";
        break;
    case DEVICE_ADC_AVERAGING_8:
        command += "8";
        break;
    case DEVICE_ADC_AVERAGING_16:
        command += "16";
        break;
    case DEVICE_ADC_AVERAGING_32:
        command += "32";
        break;
    case DEVICE_ADC_AVERAGING_64:
        command += "64";
        break;
    case DEVICE_ADC_AVERAGING_128:
        command += "128";
        break;
    case DEVICE_ADC_AVERAGING_256:
        command += "256";
        break;
    case DEVICE_ADC_AVERAGING_512:
        command += "512";
        break;
    case DEVICE_ADC_AVERAGING_1024:
        command += "1024";
        break;
    }
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    if(response != "OK"){
        return false;
    }
    adcAveraging = averagingRatio;
    return true;
}

bool Device::getAvrRatio(device_adc_averaging_t *averagingRatio)
{
    QString response;
    QString command = "device adc chavrratio get -sid=" + QString::number(streamID);
    int tmpADCAvgRatio;
    QString signalResponse =  "";
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    //Parse response
    tmpADCAvgRatio = response.toInt();
    switch(tmpADCAvgRatio)
    {
    case 1:
        adcAveraging = DEVICE_ADC_AVERAGING_DISABLED;
        signalResponse = "1";
        break;
    case 2:
        adcAveraging = DEVICE_ADC_AVERAGING_2;
        signalResponse = "2";
        break;
    case 4:
        adcAveraging = DEVICE_ADC_AVERAGING_4;
        signalResponse = "4";
        break;
    case 8:
        adcAveraging = DEVICE_ADC_AVERAGING_8;
        signalResponse = "8";
        break;
    case 16:
        adcAveraging = DEVICE_ADC_AVERAGING_16;
        signalResponse = "16";
        break;
    case 32:
        adcAveraging = DEVICE_ADC_AVERAGING_32;
        signalResponse = "32";
        break;
    case 64:
        adcAveraging = DEVICE_ADC_AVERAGING_64;
        signalResponse = "64";
        break;
    case 128:
        adcAveraging = DEVICE_ADC_AVERAGING_128;
        signalResponse = "128";
        break;
    case 256:
        adcAveraging = DEVICE_ADC_AVERAGING_256;
        signalResponse = "256";
        break;
    case 512:
        adcAveraging = DEVICE_ADC_AVERAGING_512;
        signalResponse = "512";
        break;
    case 1024:
        adcAveraging = DEVICE_ADC_AVERAGING_1024;
        signalResponse = "1024";
        break;
    default:
        adcAveraging = DEVICE_ADC_AVERAGING_UKNOWN;
        signalResponse = "0";
        break;
    }
    if(averagingRatio != NULL) *averagingRatio = adcAveraging;
    emit sigAvgRatio(signalResponse);
    return true;
}
bool Device::setSamplingTime(QString time)
{
    QString response;
    QString command = "device adc stime set -sid=" + QString::number(streamID) + " -value=";
    command += time;
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    if(response != "OK"){
        return false;
    }
    samplingTime = time;
    return true;
}

bool Device::getSamplingTime(QString *time)
{
    QString response;
    QString command = "device adc stime get -sid=" + QString::number(streamID);
    unsigned int tmpSTime;
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    //Parse response
    tmpSTime = response.toInt();
    samplingTime = response;
    if(time != NULL) *time = QString::number(tmpSTime);
    emit sigSampleTimeObtained(response);
    return true;
}

bool Device::setVOffset(QString off)
{
    QString response;
    QString command = "device adc voffset set -sid=" + QString::number(streamID) + " -value=";
    command += off;
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    if(response != "OK"){
        return false;
    }
    voltageOffset = off;
    return true;
}

bool Device::getVOffset(QString *off)
{
    QString response;
    QString command = "device adc voffset get -sid=" + QString::number(streamID);
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    //Parse response
    voltageOffset = response;
    if(off != NULL) *off = voltageOffset;
    emit sigVOffsetObtained(response);
    return true;
}

bool Device::setCOffset(QString off)
{
    QString response;
    QString command = "device adc coffset set -sid=" + QString::number(streamID) + " -value=";
    command += off;
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    if(response != "OK"){
        return false;
    }
    currentOffset = off;
    return true;
}

bool Device::getCOffset(QString *off)
{
    QString response;
    QString command = "device adc coffset get -sid=" + QString::number(streamID);
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    //Parse response
    currentOffset = response;
    if(off != NULL) *off = currentOffset;
    emit sigCOffsetObtained(response);
    return true;
}

bool Device::getADCInputClk(QString *clk)
{
    QString response;
    QString command = "device adc clk get -sid=" + QString::number(streamID);
    if(!controlLink->executeCommand(command, &response, 1000)) return false;
    //Parse response
    adcInputClk = response;
    if(clk != NULL) *clk = adcInputClk;
    emit sigAdcInputClkObtained(response);
    return true;
}

bool Device::acquireDeviceConfiguration()
{
    getResolution();
    getClockDiv();
    getSamplingTime();
    getChSampleTime();
    getVOffset();
    getCOffset();
    getAvrRatio();
    getADCInputClk();
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
