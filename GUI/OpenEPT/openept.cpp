#include <QPushButton>
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
    DeviceWnd* deviceWnd = new DeviceWnd();
    deviceWnd->setWindowTitle("Device 1");
    DeviceWnd* deviceWnd1 = new DeviceWnd();
    deviceWnd1->setWindowTitle("Device 2");

    // Add the child window to the MDI area
    QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(deviceWnd);
    QMdiSubWindow *subWindow1 = ui->mdiArea->addSubWindow(deviceWnd1);
    subWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    subWindow->show();
    subWindow1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    subWindow1->show();
}

