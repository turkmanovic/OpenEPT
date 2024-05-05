#include "dataprocessing.h"

DataProcessing::DataProcessing(QObject *parent)
    : QObject{parent}
{
    dataProcessingThread = new QThread(this);
    this->moveToThread(dataProcessingThread);
    dataProcessingThread->setObjectName("Data processing thread");
    dataProcessingThread->start();
    currentNumberOfBuffers       = 0;
    lastBufferUsedPositionIndex  = 0;
    maxNumberOfBuffers      = DATAPROCESSING_DEFAULT_NUMBER_OF_BUFFERS;
    samplesBufferSize       = DATAPROCESSING_DEFAULT_SAMPLES_BUFFER_SIZE/2;
    initVoltageBuffer();
    initCurrentBuffer();
    initKeyBuffer();
}

bool DataProcessing::setNumberOfBuffersToCollect(unsigned int numberOfBaffers)
{
    maxNumberOfBuffers = numberOfBaffers;
    initVoltageBuffer();
    initCurrentBuffer();
    initKeyBuffer();
    return true;
}

bool DataProcessing::setSamplesBufferSize(unsigned int size)
{
    samplesBufferSize = size;
    initVoltageBuffer();
    initCurrentBuffer();
    initKeyBuffer();
    return true;
}

void DataProcessing::onNewSampleBufferReceived(QVector<double> rawData, int counter, int magic)
{
    uint32_t        keyStartValue = counter*rawData.size()/2;
    unsigned int i = 0;
    for(; i < rawData.size();)
    {
        voltageDataCollected[lastBufferUsedPositionIndex] = rawData[i];
        i += 1;
        currentDataCollected[lastBufferUsedPositionIndex] = rawData[i];
        i += 1;
        if(i == 0)
        {
            keysDataCollected[lastBufferUsedPositionIndex] = keyStartValue;
        }
        else
        {
            keysDataCollected[lastBufferUsedPositionIndex] = keyStartValue + i;
        }
        lastBufferUsedPositionIndex += 1;
    }

    currentNumberOfBuffers += 1;
    if(currentNumberOfBuffers == maxNumberOfBuffers)
    {
        emit sigNewVoltageCurrentSamplesReceived(voltageDataCollected, currentDataCollected, keysDataCollected);
        initVoltageBuffer();
        initCurrentBuffer();
        initKeyBuffer();
    }
}

void DataProcessing::initVoltageBuffer()
{
    voltageDataCollected.resize(maxNumberOfBuffers*samplesBufferSize);
    voltageDataCollected.fill(0);
    currentNumberOfBuffers = 0;
    lastBufferUsedPositionIndex = 0;
}

void DataProcessing::initCurrentBuffer()
{
    currentDataCollected.resize(maxNumberOfBuffers*samplesBufferSize);
    currentDataCollected.fill(0);
    currentNumberOfBuffers = 0;
    lastBufferUsedPositionIndex = 0;
}

void DataProcessing::initKeyBuffer()
{
    keysDataCollected.resize(maxNumberOfBuffers*samplesBufferSize);
    keysDataCollected.fill(0);
    currentNumberOfBuffers = 0;
    lastBufferUsedPositionIndex = 0;
}
