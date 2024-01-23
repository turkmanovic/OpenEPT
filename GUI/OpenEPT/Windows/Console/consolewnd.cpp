#include "consolewnd.h"
#include "ui_consolewnd.h"

ConsoleWnd::ConsoleWnd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConsoleWnd)
{
    ui->setupUi(this);
    connect(ui->controlSendPusb, SIGNAL(clicked(bool)), this, SLOT(onSendControlMsgClicked()));
    connect(ui->controlSendLine, SIGNAL(returnPressed()), this, SLOT(onSendControlMsgClicked()));
    logUtil.assignLogWidget(ui->controlSendRecievePlte);
}

ConsoleWnd::~ConsoleWnd()
{
    delete ui;
}

void ConsoleWnd::printMessage(QString msg)
{
    logUtil.printLogMessage(" Response: " + msg, LOG_MESSAGE_TYPE_INFO, LOG_MESSAGE_DEVICE_TYPE_DEVICE);
}

void ConsoleWnd::onSendControlMsgClicked() {
    QString textToSend = ui->controlSendLine->text();
    logUtil.printLogMessage(" Command: " + textToSend, LOG_MESSAGE_TYPE_INFO, LOG_MESSAGE_DEVICE_TYPE_CONSOLE);
    ui->controlSendLine->clear();
    /* emit Signal to deviceWnd -> */
    emit sigControlMsgSend(textToSend);
}

void ConsoleWnd::onOkRecieved() {
    ui->controlSendRecievePlte->appendPlainText("OK");
}
