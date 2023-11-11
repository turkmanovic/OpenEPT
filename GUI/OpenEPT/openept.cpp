#include "openept.h"
#include "ui_openept.h"

OpenEPT::OpenEPT(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OpenEPT)
{
    ui->setupUi(this);
}

OpenEPT::~OpenEPT()
{
    delete ui;
}

