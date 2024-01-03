#ifndef DEVICEWND_H
#define DEVICEWND_H

#include <QWidget>
#include "Windows/Plot/plot.h"
#include "Windows/Device/advanceconfigurationwnd.h"

typedef enum
{
    DEVICE_STATE_CONNECTED,
    DEVICE_STATE_DISCONNECTED,
    DEVICE_STATE_UNDEFINED
}device_state_t;


namespace Ui {
class DeviceWnd;
}

class DeviceWnd : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceWnd(QWidget *parent = nullptr);
    ~DeviceWnd();

    QPlainTextEdit* getLogWidget();
    void            setDeviceState(device_state_t aDeviceState);

signals:
    void            sigWndClosed();
    void            sigSamplingTimeChanged(QString samplingTime);
    void            sigResolutionChanged(QString resolution);
    void            sigClockDivChanged(QString clockDiv);
    void            saveToFileEnabled(bool enableStatus);
    void            startAcquisition();
    void            pauseAcquisition();
    void            stopAcquisition();
    void            refreshAcquisition();
protected:
    void            closeEvent(QCloseEvent *event);

public slots:
    void            onAdvanceConfigurationButtonPressed(bool pressed);
    void            onClockDivCombIndexChanged(int index);
    void            onResolutionCombIndexChanged(int index);
    void            onSamplingTimeCombIndexChanged(int index);
    //void            onInfoSaveToFileEnabled(bool enableStatus);
    void            onSaveToFileChanged(int value);
    void            onStartAcquisition();
    void            onPauseAcquisition();
    void            onStopAcquisiton();
    void            onRefreshAcquisiton();

private:
    Ui::DeviceWnd               *ui;

    AdvanceConfigurationWnd     *advanceConfigurationWnd;

    Plot                        *voltageChart;
    Plot                        *currentChart;
    Plot                        *consumptionChart;

    QStringList                 sampleTimeOptions;
    QStringList                 resolutionOptions;
    QStringList                 clockDivOptions;
    QStringList                 sampleAveraginOptions;

    /* File info */
    bool                        saveToFileFlag;

    /* */
    device_state_t              deviceState;

    /* */
    void            setDeviceStateDisconnected();
    void            setDeviceStateConnected();
};

#endif // DEVICEWND_H
