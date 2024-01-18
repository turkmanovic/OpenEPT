#include "consolewnd.h"
#include "ui_consolewnd.h"

ConsoleWnd::ConsoleWnd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConsoleWnd)
{
    ui->setupUi(this);
    connect(ui->controlSendPusb, SIGNAL(clicked(bool)), this, SLOT(onSendClicked()));
    connect(ui->controlSendLine, SIGNAL(returnPressed()), this, SLOT(onSendClicked()));
    logUtil.assignLogWidget(ui->controlSendRecievePlte);
}

ConsoleWnd::~ConsoleWnd()
{
    delete ui;
}

void ConsoleWnd::printMessage(QString msg)
{
    logUtil.printLogMessage(" Response: " + msg, LOG_MESSAGE_TYPE_INFO);
}

void ConsoleWnd::onSendClicked() {
    QString textToSend = ui->controlSendLine->text();
    logUtil.printLogMessage(" Command: " + textToSend, LOG_MESSAGE_TYPE_INFO, LOG_MESSAGE_DEVICE_TYPE_CONSOLE);
    emit onHelloSend(textToSend);
}

void ConsoleWnd::onOkRecieved() {
    ui->controlSendRecievePlte->appendPlainText("OK");
}
