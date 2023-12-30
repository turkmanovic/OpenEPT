#ifndef CONTROLLINK_H
#define CONTROLLINK_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

#define CONTROL_LINK_COMMAND_TIMEOUT    1000

typedef enum{
    CONTROL_LINK_STATUS_ESTABLISHED,
    CONTROL_LINK_STATUS_DISABLED
}control_link_status_t;

class ControlLink : public QObject
{
    Q_OBJECT
public:
    explicit ControlLink(QObject *parent = nullptr);
    control_link_status_t   establishLink(QString aIpAddress, QString aPortNumber);
    bool                    getDeviceName(QString* deviceName);

signals:
    void                    sigDisconnected();
    void                    sigConnected();

public slots:

    void                    onDisconnected();
    void                    onReconnected();

private:

    QTcpSocket              *tcpSocket;
    QString                 ipAddress;
    quint16                 portNumber;
    control_link_status_t   linkStatus;


    bool                    executeCommand(QString command, QString* response, int timeout);
    bool                    setSocketKeepAlive();

};

#endif // CONTROLLINK_H
