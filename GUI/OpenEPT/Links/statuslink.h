#ifndef STATUSLINK_H
#define STATUSLINK_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>

#define STATUS_LINK_SERVER_PORT         10000
#define STATUS_LINK_BUFFER_SIZE         1500

class StatusLink : public QObject
{
    Q_OBJECT
public:
    explicit StatusLink(QObject *parent = nullptr);

    void                    startServer();

signals:
    void                    sigNewStatusMessageReceived(QString message);

public slots:
    void                    onServerStarted();
    void                    onReadPendingData();


private:
    QTcpSocket              *tcpServer;
    QThread                 *tcpServerThread;
};

#endif // STATUSLINK_H
