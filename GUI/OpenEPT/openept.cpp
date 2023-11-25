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

//    AddDeviceWnd = new adddevicewnd();
//    AddDeviceWnd->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    AddDeviceWnd->show();


 }

OpenEPT::~OpenEPT()
{
    delete ui;
}
