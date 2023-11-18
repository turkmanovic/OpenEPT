#include "openept.h"
#include "ui_openept.h"
#include "ui_devicewnd.h"  // Include the generated UI header for the devicewnd.ui file
#include "devicewnd.h"

OpenEPT::OpenEPT(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OpenEPT)
{
    ui->setupUi(this);

    // Create MDI area
    mdiArea = new QMdiArea(this);
    setCentralWidget(mdiArea);

    // Load the UI file for the child window
//    QUiLoader loader;
//    QFile file("D:/prof_Hari/OpenEPT/GUI/OpenEPT/devicewnd.ui");  // Adjust the path to your devicewnd.ui file
//    if (!file.open(QFile::ReadOnly)) {
//        QMessageBox::warning(this, "Error", "Could not open UI file");
//        return;
//    }

//    QWidget *childWidget = loader.load(&file, this);
//    file.close();

//    if (!childWidget) {
//        QMessageBox::warning(this, "Error", "Could not create widget from UI file");
//        return;
//    }
    DeviceWnd* deviceWnd = new DeviceWnd();




    // Add the child window to the MDI area
    QMdiSubWindow *subWindow = mdiArea->addSubWindow(deviceWnd);
    subWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    subWindow->show();

    // Create the menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // Create File menu
    QMenu *fileMenu = menuBar->addMenu("File");

    QAction *openAction = new QAction("Open", this);
    fileMenu->addAction(openAction);

    QAction *saveAction = new QAction("Save", this);
    fileMenu->addAction(saveAction);

    // Create Devices menu
    QMenu *devicesMenu = menuBar->addMenu("Devices");

    // Create About menu
    QMenu *aboutMenu = menuBar->addMenu("About");

    QAction *aboutAction = new QAction("About OpenEPT", this);
    aboutMenu->addAction(aboutAction);
}

OpenEPT::~OpenEPT()
{
    delete ui;
}
