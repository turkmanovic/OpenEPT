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
    void    onDeviceControlLinkDisconnected();
    void    onDeviceControlLinkConnected();
    void    onDeviceStatusLinkNewDeviceAdded(QString aDeviceIP);
    void    onDeviceStatusLinkNewMessageReceived(QString aDeviceIP, QString aMessage);
    void    onDeviceClosed();
    void    onConsoleWndMessageRcvd(QString msg);
    void    onConsoleWndHandleControlMsgResponse(QString msg);
    void    onResolutionChanged(QString resolution);
    void    onClockDivChanged(QString clockDiv);
    void    onSampleTimeChanged(QString stime);
    void    onSamplingTimeChanged(QString time);
    void    onInterfaceChanged(QString interfaceIp);
    void    onAvrRatioChanged(QString avgRatio);
    void    onVOffsetChanged(QString off);
    void    onCOffsetChanged(QString off);
    void    onAcquisitionStart();
    void    onAcquisitionStop();
    void    onAcquisitionPause();

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
