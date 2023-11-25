#ifndef CONTAINER_H
#define CONTAINER_H

#include "device.h"
#include "Windows/Device/devicewnd.h"


class Container
{
public:
    Container();

private:
    DeviceWnd*  deviceWnd;
    Device*     deviceData;


};

#endif // CONTAINER_H
