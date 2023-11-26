#ifndef OPENEPT_H
#define OPENEPT_H

#include <QMainWindow>
#include <QList>
#include <QMessageBox>
#include "Windows/AddDevice/adddevicewnd.h"
#include "devicecontainer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class OpenEPT; }
QT_END_NAMESPACE

class OpenEPT : public QMainWindow
{
    Q_OBJECT

public:
    OpenEPT(QWidget *parent = nullptr);
    ~OpenEPT();


private slots:
    void onActionAddSingleDeviceTriggered();
    void onAddDeviceWndAddDevice(QString aIpAddress, QString aPort);
    void onDeviceContainerDeviceWndClosed(Device* aDevice);

private:
    Ui::OpenEPT                 *ui;
    /**/
    AddDeviceWnd                *addDeviceWnd;
    /**/
    QList<DeviceContainer*>     deviceList;
    /**/
    QMenu                       *connectedDevicesMenu;

    /* */
    QMessageBox                 msgBox;

    /**/
    bool                        addNewDevice(QString aIpAddress, QString aPort);

};
#endif // OPENEPT_H
