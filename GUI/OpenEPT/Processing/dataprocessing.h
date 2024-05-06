#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <QObject>
#include <QThread>


#define DATAPROCESSING_DEFAULT_NUMBER_OF_BUFFERS        100
#define DATAPROCESSING_DEFAULT_SAMPLES_BUFFER_SIZE      500
#define DATAPROCESSING_DEFAULT_SAMPLE_SIZE              2

typedef enum
{
    DATAPROCESSING_ACQUISITION_STATUS_ACTIVE,
    DATAPROCESSING_ACQUISITION_STATUS_INACTIVE
}dataprocessing_acquisition_status_t;

class DataProcessing : public QObject
{
    Q_OBJECT
public:
    explicit                            DataProcessing(QObject *parent = nullptr);
    bool                                setNumberOfBuffersToCollect(unsigned int numberOfBaffers);
    bool                                setSamplesBufferSize(unsigned int size);
    bool                                setSamplingPeriod(double aSamplingPeriod);  //us
    bool                                setSamplingTime(double aSamplingTime);      //us

    bool                                setAcquisitionStatus(dataprocessing_acquisition_status_t aAcquisitionStatus);

signals:
    void                                sigNewVoltageCurrentSamplesReceived(QVector<double> voltage, QVector<double> current, QVector<double> voltageKeys, QVector<double> currentKeys);
    void                                sigSamplesBufferReceiveStatistics(double dropRate, unsigned int fullPacketCounter, unsigned int lastPacketID);


public slots:
    void                                onNewSampleBufferReceived(QVector<double> rawData, int packetID,int magic);

private:
    void                                initBuffers();
    void                                initVoltageBuffer();
    void                                initCurrentBuffer();
    void                                initKeyBuffer();

    /* Stream link received data in separate worker thread*/
    QThread                             *dataProcessingThread;

    /* */
    unsigned int                        currentNumberOfBuffers;
    unsigned int                        lastBufferUsedPositionIndex;
    unsigned int                        receivedPacketCounter;      //During one session
    unsigned int                        lastReceivedPacketID;
    unsigned int                        dropPacketsNo;          //Number of dropped packets
    bool                                firstPacketReceived;    //

    /* */
    double                              samplingPeriod;         //ms
    double                              samplingTime;           //ms

    /* */
    unsigned int                        maxNumberOfBuffers;
    unsigned int                        samplesBufferSize;

    /* */
    QVector<double>                     voltageDataCollected;
    QVector<double>                     currentDataCollected;
    QVector<double>                     voltageKeysDataCollected;
    QVector<double>                     currentKeysDataCollected;


    /**/
    dataprocessing_acquisition_status_t acquisitionStatus;

};

#endif // DATAPROCESSING_H
