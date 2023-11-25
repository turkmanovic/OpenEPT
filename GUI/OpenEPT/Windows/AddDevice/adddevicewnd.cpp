#include "adddevicewnd.h"
#include "ui_adddevicewnd.h"

adddevicewnd::adddevicewnd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::adddevicewnd)
{
    ui->setupUi(this);  
}

adddevicewnd::~adddevicewnd()
{
    delete ui;
}
