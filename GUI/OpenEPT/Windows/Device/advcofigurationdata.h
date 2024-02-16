#ifndef ADVCOFIGURATIONDATA_H
#define ADVCOFIGURATIONDATA_H

#include <QString>
#include <QMetaType>

struct advConfigurationData {
    QString resolution;
    QString clockDiv;
    QString sampleTime;
    QString averaginRatio;
    QString voltageOffset;
    QString currentOffset;
};

Q_DECLARE_METATYPE(advConfigurationData)
#endif // ADVCOFIGURATIONDATA_H
