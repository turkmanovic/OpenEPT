#ifndef CONTROLLINK_H
#define CONTROLLINK_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

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

signals:

public slots:

    void                    onDisconnected();
    void                    onReconnected();

private:

    QTcpSocket              *tcpSocket;
    QString                 ipAddress;
    quint16                 portNumber;
    control_link_status_t   linkStatus;


    QString                 executeCommand(QString command, int timeout);

};

#endif // CONTROLLINK_H
