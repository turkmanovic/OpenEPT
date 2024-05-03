#include <QDataStream>
#include <QNetworkDatagram>
#include "streamlink.h"

StreamLink::StreamLink(QObject *parent)
    : QObject{parent}
{
    port = 0;
    id  = 0;
    udpThread = new QThread(this);
    this->moveToThread(udpThread);
    udpThread->setObjectName("UDP Thread");
    QObject::connect(udpThread, &QThread::started, this, &StreamLink::initStreamLinkThread, Qt::QueuedConnection);
}

void StreamLink::setPort(quint16 aPort)
{
    port = aPort;
}

void StreamLink::setID(unsigned int aID)
{
    id = aID;
}

unsigned int StreamLink::getID()
{
    return id;
}

void StreamLink::enable()
{
    udpThread->start();
}

void StreamLink::initStreamLinkThread()
{
    udpSocket = new QUdpSocket();
    udpSocket->bind(port);
    udpSocket->setReadBufferSize(8000000);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingData()));
}

void StreamLink::readPendingData()
{
    unsigned int counter;
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray receivedData;
        QVector<unsigned short> data;
        QVector<double> data_double;

        receivedData.resize(udpSocket->pendingDatagramSize());
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        receivedData = datagram.data();

        data.resize(STREAM_LINK_PACKET_SIZE);
        memcpy(data.data(), receivedData.data()+4, STREAM_LINK_PACKET_SIZE*2);
        memcpy(&counter, receivedData.data(), 4);
        data_double.reserve(data.size());
        std::copy(data.cbegin(), data.cend(), std::back_inserter(data_double));
        emit newDataRecevied(data_double, counter);
    }
}
