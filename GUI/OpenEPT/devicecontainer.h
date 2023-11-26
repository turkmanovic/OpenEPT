#ifndef DEVICECONTAINER_H
#define DEVICECONTAINER_H

#include <QObject>
#include "Windows/Device/devicewnd.h"
#include "device.h"

class DeviceContainer : public QObject
{
    Q_OBJECT
public:
    explicit DeviceContainer(QObject *parent = nullptr,  DeviceWnd* aDeviceWnd = nullptr, Device* aDevice = nullptr);

signals:

private:
    DeviceWnd*  deviceWnd;
    Device*     device;

};

#endif // DEVICECONTAINER_H
