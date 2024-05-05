#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <QObject>
#include <QThread>


#define DATAPROCESSING_DEFAULT_NUMBER_OF_BUFFERS        100
#define DATAPROCESSING_DEFAULT_SAMPLES_BUFFER_SIZE      500
#define DATAPROCESSING_DEFAULT_SAMPLE_SIZE              2

class DataProcessing : public QObject
{
    Q_OBJECT
public:
    explicit        DataProcessing(QObject *parent = nullptr);
    bool            setNumberOfBuffersToCollect(unsigned int numberOfBaffers);
    bool            setSamplesBufferSize(unsigned int size);

signals:
    void            sigNewVoltageCurrentSamplesReceived(QVector<double> voltage, QVector<double> current, QVector<double> keys);


public slots:
    void            onNewSampleBufferReceived(QVector<double> rawData, int counter,int magic);

private:
    void            initVoltageBuffer();
    void            initCurrentBuffer();
    void            initKeyBuffer();
    /* Stream link received data in separate worker thread*/
    QThread         *dataProcessingThread;

    /* */
    unsigned int    currentNumberOfBuffers;
    unsigned int    lastBufferUsedPositionIndex;
    unsigned int    maxNumberOfBuffers;
    unsigned int    samplesBufferSize;

    /**/
    QVector<double> voltageDataCollected;
    QVector<double> currentDataCollected;
    QVector<double> keysDataCollected;

};

#endif // DATAPROCESSING_H
