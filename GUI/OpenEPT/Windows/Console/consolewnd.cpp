#include "consolewnd.h"
#include "ui_consolewnd.h"

ConsoleWnd::ConsoleWnd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConsoleWnd)
{
    ui->setupUi(this);
    connect(ui->controlSendPusb, SIGNAL(clicked(bool)), this, SLOT(onSendClicked()));
    connect(ui->controlSendLine, SIGNAL(returnPressed()), this, SLOT(onSendClicked()));
}

ConsoleWnd::~ConsoleWnd()
{
    delete ui;
}

void ConsoleWnd::onSendClicked() {
    QString textToSend = ui->controlSendLine->text();
    ui->controlSendRecievePlte->appendPlainText(textToSend);
    ui->controlSendLine->clear();
}
