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

    // Add the child window to the MDI area
    QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(deviceWnd);
    subWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    subWindow->show();

 }

OpenEPT::~OpenEPT()
{
    delete ui;
}
