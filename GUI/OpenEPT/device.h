#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>

typedef enum{
    DEVICE_ADC_RESOLUTION_UKNOWN       = 0,
    DEVICE_ADC_RESOLUTION_16BIT        = 16,
    DEVICE_ADC_RESOLUTION_14BIT        = 14,
    DEVICE_ADC_RESOLUTION_12BIT        = 12,
    DEVICE_ADC_RESOLUTION_10BIT        = 10
}device_adc_resolution_t;

typedef enum{
    DEVICE_ADC_SAMPLING_SPEED_UKNOWN   = 0,
    DEVICE_ADC_SAMPLING_SPEED_1C5      = 1,
    DEVICE_ADC_SAMPLING_SPEED_2C5      = 2,
    DEVICE_ADC_SAMPLING_SPEED_8C5      = 8,
    DEVICE_ADC_SAMPLING_SPEED_16C5     = 16,
    DEVICE_ADC_SAMPLING_SPEED_32C5     = 32,
    DEVICE_ADC_SAMPLING_SPEED_64C5     = 64,
    DEVICE_ADC_SAMPLING_SPEED_387C5    = 387,
    DEVICE_ADC_SAMPLING_SPEED_810C5    = 810
}device_adc_sampling_speed_t;

typedef enum{
    DEVICE_ADC_CLOCK_DIV_UKNOWN        = 0,
    DEVICE_ADC_CLOCK_DIV_1             = 1,
    DEVICE_ADC_CLOCK_DIV_2             = 2,
    DEVICE_ADC_CLOCK_DIV_4             = 4,
    DEVICE_ADC_CLOCK_DIV_6             = 6,
    DEVICE_ADC_CLOCK_DIV_8             = 8,
    DEVICE_ADC_CLOCK_DIV_10            = 10,
    DEVICE_ADC_CLOCK_DIV_12            = 12,
    DEVICE_ADC_CLOCK_DIV_16            = 16,
    DEVICE_ADC_CLOCK_DIV_32            = 32,
    DEVICE_ADC_CLOCK_DIV_64            = 64,
    DEVICE_ADC_CLOCK_DIV_128           = 128,
    DEVICE_ADC_CLOCK_DIV_256           = 256
}device_adc_clock_div_t;

typedef enum{
    DEVICE_ADC_AVERAGING_UKNOWN        = 0,
    DEVICE_ADC_AVERAGING_DISABLED      = 1,
    DEVICE_ADC_AVERAGING_2             = 2,
    DEVICE_ADC_AVERAGING_4             = 4,
    DEVICE_ADC_AVERAGING_8             = 8,
    DEVICE_ADC_AVERAGING_16            = 16,
    DEVICE_ADC_AVERAGING_32            = 32,
    DEVICE_ADC_AVERAGING_64            = 64,
    DEVICE_ADC_AVERAGING_128           = 128,
    DEVICE_ADC_AVERAGING_256           = 256,
    DEVICE_ADC_AVERAGING_512           = 512,
    DEVICE_ADC_AVERAGING_1024          = 1024
}device_adc_averaging_t;

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr);

signals:

private:
    device_adc_resolution_t     adcResolution;
    device_adc_sampling_speed_t adcSamplingSpeed;
    device_adc_clock_div_t      adcClockingDiv;
    device_adc_averaging_t      adcAveraging;

};

#endif // DEVICE_H
