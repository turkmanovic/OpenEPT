#include "controllink.h"
#include "Ws2tcpip.h"
#include "WinSock2.h"

ControlLink::ControlLink(QObject *parent)
    : QObject{parent}
{
    tcpSocket       =   new QTcpSocket(this);
    tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    ipAddress       =   "0.0.0.0";
    portNumber      =   0;
    linkStatus      =   CONTROL_LINK_STATUS_DISABLED;
}

control_link_status_t   ControlLink::establishLink(QString aIpAddress, QString aPortNumber)
{
    QHostAddress    hostAddress(aIpAddress);
    qint16          hostPort = aPortNumber.toUShort();
    tcpSocket->connectToHost(hostAddress, hostPort);
    if(tcpSocket->waitForConnected(1000)){
        ipAddress = aIpAddress;
        portNumber = hostPort;

        //setSocketKeepAlive();

        linkStatus = CONTROL_LINK_STATUS_ESTABLISHED;
        connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect()),Qt::QueuedConnection);
        connect(tcpSocket, SIGNAL(connected()), this, SLOT(onReconnected()),Qt::QueuedConnection);
    }
    return linkStatus;
}
QString             ControlLink::executeCommand(QString command, int timeout)
{
    QByteArray  receivedData;
    QByteArray  dataToSend(command.toUtf8());
    QString     receivedResponse = "";
    receivedData.clear();
    if(linkStatus != CONTROL_LINK_STATUS_ESTABLISHED) return receivedResponse;
    tcpSocket->flush();
    tcpSocket->write(dataToSend);
    tcpSocket->waitForBytesWritten();
    if(tcpSocket->waitForReadyRead(timeout) != true) return receivedResponse;
    receivedData = tcpSocket->readAll();
    QString responseAsString(receivedData);
    QStringList responseParts = responseAsString.split( " " );
    if(responseParts[0] == "ok" && responseParts.size() > 1){
        for(int i = 1; i < responseParts.size(); i++){
            receivedResponse += responseParts[i];
            if(i != (responseParts.size() - 1)){
                receivedResponse += " ";
            }
        }
    }
    else {
        if(responseParts.size() == 1){
            receivedResponse = responseParts[0];
        }
    }
    return receivedResponse;
}
void ControlLink::onDisconnected()
{
    linkStatus = CONTROL_LINK_STATUS_DISABLED;

}

void ControlLink::onReconnected()
{
    linkStatus = CONTROL_LINK_STATUS_ESTABLISHED;

}
