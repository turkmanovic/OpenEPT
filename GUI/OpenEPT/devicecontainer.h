#ifndef DEVICECONTAINER_H
#define DEVICECONTAINER_H

#include <QObject>
#include "device.h"
#include "Windows/Device/devicewnd.h"
#include "Utility/log.h"

class DeviceContainer : public QObject
{
    Q_OBJECT
public:
    explicit DeviceContainer(QObject *parent = nullptr,  DeviceWnd* aDeviceWnd = nullptr, Device* aDevice = nullptr);
    ~DeviceContainer();

signals:
    void    sigDeviceClosed(Device* device);

public slots:
    void    onConsoleWndMessageRcvd(QString msg);
    void    onDeviceWndResolutionChanged(QString resolution);
    void    onDeviceWndClockDivChanged(QString clockDiv);
    void    onDeviceWndSampleTimeChanged(QString stime);
    void    onDeviceWndSamplingTimeChanged(QString time);
    void    onDeviceWndInterfaceChanged(QString interfaceIp);
    void    onDeviceWndAvrRatioChanged(QString avgRatio);
    void    onDeviceWndVOffsetChanged(QString off);
    void    onDeviceWndCOffsetChanged(QString off);
    void    onDeviceWndAcquisitionStart();
    void    onDeviceWndAcquisitionStop();
    void    onDeviceWndAcquisitionPause();
    void    onDeviceWndAdvConfGet();
    void    onDeviceWndAcquisitionRefresh();
    void    onDeviceWndNewConfiguration(QVariant newConfig);


    void    onDeviceWndClosed();
    void    onDeviceControlLinkDisconnected();
    void    onDeviceControlLinkConnected();
    void    onDeviceStatusLinkNewDeviceAdded(QString aDeviceIP);
    void    onDeviceStatusLinkNewMessageReceived(QString aDeviceIP, QString aMessage);
    void    onDeviceHandleControlMsgResponse(QString msg);
    void    onDeviceResolutionObtained(QString resolution);
    void    onDeviceClkDivObtained(QString clkDiv);
    void    onDeviceChSampleTimeObtained(QString stime);
    void    onDeviceSTimeObtained(QString stime);
    void    onDeviceAdcInClkObtained(QString inClk);
    void    onDeviceAvgRatioChanged(QString aAvgRatio);
    void    onDeviceCOffsetObtained(QString coffset);
    void    onDeviceVOffsetObtained(QString voffset);

    void    onDeviceNewVoltageCurrentSamplesReceived(QVector<double> voltage, QVector<double> current, QVector<double> keys);



private:
    DeviceWnd*  deviceWnd;
    Device*     device;
    Log*        log;


    device_adc_resolution_t         getAdcResolutionFromString(QString resolution);
    device_adc_clock_div_t          getAdcClockDivFromString(QString clkDiv);
    device_adc_ch_sampling_time_t   getAdcChSamplingTimeFromString(QString chstime);
    device_adc_averaging_t          getAdcAvgRatioFromString(QString avgRatio);

};

#endif // DEVICECONTAINER_H
