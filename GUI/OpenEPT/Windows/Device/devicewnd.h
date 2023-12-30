#ifndef DEVICEWND_H
#define DEVICEWND_H

#include <QWidget>
#include "Windows/Plot/plot.h"
#include "Windows/Device/advanceconfigurationwnd.h"


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

signals:
    void    sigWndClosed();
    void    sigSamplingTimeChanged(QString samplingTime);
    void    sigResolutionChanged(QString resolution);
    void    sigClockDivChanged(QString clockDiv);
    void    saveToFileEnabled(bool enableStatus);
    void    startAcquisition();
    void    pauseAcquisition();
    void    stopAcquisition();
    void    refreshAcquisition();
protected:
    void    closeEvent(QCloseEvent *event);

public slots:
    void    onAdvanceConfigurationButtonPressed(bool pressed);
    void    onClockDivCombIndexChanged(int index);
    void    onResolutionCombIndexChanged(int index);
    void    onSamplingTimeCombIndexChanged(int index);
    //void    onInfoSaveToFileEnabled(bool enableStatus);
    void    onSaveToFileChanged(int value);
    void    onStartAcquisition();
    void    onPauseAcquisition();
    void    onStopAcquisiton();
    void    onRefreshAcquisiton();

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
};

#endif // DEVICEWND_H
