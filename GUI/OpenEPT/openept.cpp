#include "openept.h"
#include "ui_openept.h"
#include "ui_devicewnd.h"  // Include the generated UI header for the devicewnd.ui file

OpenEPT::OpenEPT(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OpenEPT)
{
    ui->setupUi(this);

    // Create MDI area
    mdiArea = new QMdiArea(this);
    setCentralWidget(mdiArea);

    // Load the UI file for the child window
    QUiLoader loader;
    QFile file("D:/prof_Hari/OpenEPT/GUI/OpenEPT/devicewnd.ui");  // Adjust the path to your devicewnd.ui file
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Could not open UI file");
        return;
    }

    QWidget *childWidget = loader.load(&file, this);
    file.close();

    if (!childWidget) {
        QMessageBox::warning(this, "Error", "Could not create widget from UI file");
        return;
    }

//    // Find the grid layout in the loaded UI
//    QGridLayout *deviceWndGridLayout = childWidget->findChild<QGridLayout*>("DeviceWndGrid");

//    if (!deviceWndGridLayout) {
//        QMessageBox::warning(this, "Error", "Could not find DeviceWndGrid layout");
//        return;
//    }

    // Set size policy for the grid layout
    //deviceWndGridLayout->setSizeConstraint(QLayout::SetFixedSize);


    // Add the child window to the MDI area
    QMdiSubWindow *subWindow = mdiArea->addSubWindow(childWidget);
    subWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    subWindow->show();
}

OpenEPT::~OpenEPT()
{
    delete ui;
}
