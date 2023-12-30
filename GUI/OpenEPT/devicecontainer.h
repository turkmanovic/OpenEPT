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

signals:
    void    sigDeviceClosed(Device* device);

public slots:
    void    onDeviceControlLinkDisconnected();
    void    onDeviceControlLinkConnected();
    void    onDeviceClosed();

private:
    DeviceWnd*  deviceWnd;
    Device*     device;
    Log*        log;

};

#endif // DEVICECONTAINER_H
