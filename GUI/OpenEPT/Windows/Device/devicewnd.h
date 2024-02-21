#ifndef DEVICEWND_H
#define DEVICEWND_H

#include <QWidget>
#include "Windows/Plot/plot.h"
#include "Windows/Device/advanceconfigurationwnd.h"
#include "Windows/Console/consolewnd.h"
#include "Windows/Device/advcofigurationdata.h"

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
    void            printConsoleMsg(QString msg);

signals:
    void            sigWndClosed();
    void            sigSamplingTimeChanged(QString time);
    void            sigResolutionChanged(int resolution);
    void            sigClockDivChanged(int clockDiv);
    void            sigSampleTimeChanged(int sampleTime);
    void            sigAvrRatioChanged(int index);
    void            sigVOffsetChanged(QString off);
    void            sigCOffsetChanged(QString off);
    void            saveToFileEnabled(bool enableStatus);
    void            startAcquisition();
    void            pauseAcquisition();
    void            stopAcquisition();
    void            refreshAcquisition();
    void            sigNewControlMessageRcvd(const QString &response);
protected:
    void            closeEvent(QCloseEvent *event);

public slots:
    void            onAdvanceConfigurationButtonPressed(bool pressed);
    //void            onClockDivCombIndexChanged(int index);
    //void            onResolutionCombIndexChanged(int index);
    //void            onSamplingTimeCombIndexChanged(int index);
    //void            onInfoSaveToFileEnabled(bool enableStatus);
    void            onSaveToFileChanged(int value);
    void            onStartAcquisition();
    void            onPauseAcquisition();
    void            onStopAcquisiton();
    void            onRefreshAcquisiton();
    void            onConsolePressed();
    void            onNewControlMsgRcvd(QString text);
    void            onPathInfo();

    void            onResolutionChanged(int index);
    void            onClockDivChanged(int index);
    void            onSampleTimeChanged(int index);
    void            onSamplingTimeChanged();
    void            onSamplingTimeTxtChanged(QString time);
    void            onAdvResolutionChanged(int index);
    void            onAdvClockDivChanged(int index);
    void            onAdvSampleTimeChanged(int index);
    void            onAdvSamplingTimeChanged(QString time);
    void            onAdvConfigurationChanged(QVariant data);

private:
    Ui::DeviceWnd               *ui;

    AdvanceConfigurationWnd     *advanceConfigurationWnd;

    ConsoleWnd                  *consoleWnd;
    Plot                        *voltageChart;
    Plot                        *currentChart;
    Plot                        *consumptionChart;

    QStringList*                sampleTimeOptions;
    QStringList*                resolutionOptions;
    QStringList*                clockDivOptions;
    QStringList                 sampleAveraginOptions;
    QString                     samplingTime;

    /* File info */
    bool                        saveToFileFlag;

    bool                        samplingTextChanged = false;

    /* */
    device_state_t              deviceState;

    /* */
    void            setDeviceStateDisconnected();
    void            setDeviceStateConnected();
    void            onAvrRatioChanged(int index);
    void            onVOffsetChanged(QString off);
    void            onCOffsetChanged(QString off);
};

#endif // DEVICEWND_H
