#include "statuslink.h"

StatusLink::StatusLink(QObject *parent)
    : QObject{parent}
{
    tcpServerThread = new QThread(this);
    connect(tcpServerThread, SIGNAL(started()),this,SLOT(onServerStarted()));
}

void StatusLink::startServer()
{
    tcpServerThread->start();
}

void StatusLink::onServerStarted()
{
    tcpServer   = new QTcpSocket();
    tcpServer->bind(STATUS_LINK_SERVER_PORT);
    connect(tcpServer, SIGNAL(readyRead()), this, SLOT(onReadPendingData()));

}

void StatusLink::onReadPendingData()
{
    char message[STATUS_LINK_BUFFER_SIZE];
    memset(message, 0, STATUS_LINK_BUFFER_SIZE);
}
