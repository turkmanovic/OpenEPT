#ifndef ADVCOFIGURATIONDATA_H
#define ADVCOFIGURATIONDATA_H

#include <QString>
#include <QMetaType>

typedef struct{
    QString resolution;
    QString clockDiv;
    QString sampleTime;
    QString samplingTime;
    int     averaginRatioIndex;
    QString voltageOffset;
    QString currentOffset;
}advConfigurationData;

Q_DECLARE_METATYPE(advConfigurationData)
#endif // ADVCOFIGURATIONDATA_H
