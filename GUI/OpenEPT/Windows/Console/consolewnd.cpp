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
    lastIndex = 0;
}

ConsoleWnd::~ConsoleWnd()
{
    delete ui;
}

void ConsoleWnd::printMessage(QString msg, bool exeStatus)
{
    if(exeStatus)
    {
        logUtil.printLogMessage(" Response: " + msg, LOG_MESSAGE_TYPE_INFO, LOG_MESSAGE_DEVICE_TYPE_DEVICE);
    }
    else
    {
        logUtil.printLogMessage(" Response: " + msg, LOG_MESSAGE_TYPE_ERROR, LOG_MESSAGE_DEVICE_TYPE_DEVICE);
    }
}

void ConsoleWnd::onSendControlMsgClicked() {
    QString textToSend = ui->controlSendLine->text();
    logUtil.printLogMessage(" Command: " + textToSend, LOG_MESSAGE_TYPE_INFO, LOG_MESSAGE_DEVICE_TYPE_CONSOLE);
    entries.append(textToSend);
    if(entries.length() != 0)
    {
        lastIndex = entries.length();
    }
    ui->controlSendLine->clear();
    /* emit Signal to deviceWnd -> */
    emit sigControlMsgSend(textToSend);
}

void ConsoleWnd::onOkRecieved() {
    ui->controlSendRecievePlte->appendPlainText("OK");
}
void ConsoleWnd::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Up)
    {
        if(entries.length() != 0){
            lastIndex = lastIndex - 1;
            lastIndex = lastIndex < 0 ?  0: lastIndex;
            ui->controlSendLine->setText(entries.at(lastIndex));
        }

    }
    if(event->key() == Qt::Key_Down)
    {
        if(entries.length() != 0){
            lastIndex = lastIndex + 1;
            lastIndex = lastIndex >= entries.length() ?  entries.length() - 1 : lastIndex;
            ui->controlSendLine->setText(entries.at(lastIndex));
        }
    }
}
