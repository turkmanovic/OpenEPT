#include <QPushButton>
#include <QAction>
#include <QMenu>
#include "openept.h"
#include "Windows/Device/devicewnd.h"
#include "ui_openept.h"


#define BUTTON_WIDTH (25)


OpenEPT::OpenEPT(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OpenEPT)
{
    ui->setupUi(this);



    addDeviceWnd = new AddDeviceWnd(this);
    addDeviceWnd->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    addDeviceWnd->setWindowModality(Qt::WindowModal);
    connect(addDeviceWnd, SIGNAL(sigAddDevice(QString,QString)), this, SLOT(onAddDeviceWndAddDevice(QString,QString)), Qt::QueuedConnection);


    connectedDevicesMenu = new QMenu("Connected devices");
    ui->menuDevices->addMenu(connectedDevicesMenu);


    connect(ui->actionAddSingleDevice, &QAction::triggered, this,  &OpenEPT::onActionAddSingleDeviceTriggered);


 }

OpenEPT::~OpenEPT()
{
    delete ui;
}

void OpenEPT::onActionAddSingleDeviceTriggered()
{
    addDeviceWnd->show();
}

void OpenEPT::onAddDeviceWndAddDevice(QString aIpAddress, QString aPort)
{
    if(addNewDevice(aIpAddress, aPort))
    {
        msgBox.setText("Device sucessfully added");
        msgBox.exec();
    }
    else
    {
        msgBox.setText("Unable to add device");
        msgBox.exec();
    }
}

bool OpenEPT::addNewDevice(QString aIpAddress, QString aPort)
{
    /* Create corresponding device window*/
    DeviceWnd *tmpdeviceWnd = new DeviceWnd();
    QString deviceName = "Device Name " + QString::number(deviceList.size());
    tmpdeviceWnd->setWindowTitle(deviceName);

    /* Create device container */
    DeviceContainer *tmpDeviceContainer = new DeviceContainer(NULL,tmpdeviceWnd);

    /* Add device to menu bar */
    QAction* tmpDeviceAction = new QAction(deviceName);
    connectedDevicesMenu->addAction(tmpDeviceAction);

    // Add the child window to the MDI area
    QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(tmpdeviceWnd);
    subWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    subWindow->show();

    deviceList.append(tmpDeviceContainer);

    return true;
}

