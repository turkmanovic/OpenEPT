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
    tcpServer   = new QTcpServer();

    if(!tcpServer->listen(QHostAddress::Any, STATUS_LINK_SERVER_PORT))
    {
        qDebug()<<"Status Link Server Bind failed";
    }
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnectionAdded()));

}

void StatusLink::onNewConnectionAdded()
{
    QTcpSocket* tmpSocket;
    while(tcpServer->hasPendingConnections())
    {
        tmpSocket = tcpServer->nextPendingConnection();
        emit sigNewClientConnected(QHostAddress(tmpSocket->peerAddress().toIPv4Address()).toString());
    }
}

void StatusLink::onReadPendingData()
{
    char message[STATUS_LINK_BUFFER_SIZE];
    memset(message, 0, STATUS_LINK_BUFFER_SIZE);
}
