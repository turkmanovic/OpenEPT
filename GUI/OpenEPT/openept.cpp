#include "openept.h"
#include "ui_openept.h"
#include "devicewnd.h"
#include <QPushButton>


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

 }

OpenEPT::~OpenEPT()
{
    delete ui;
}
