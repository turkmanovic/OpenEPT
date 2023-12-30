#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QPlainTextEdit>

typedef enum
{
    LOG_MESSAGE_TYPE_INFO,
    LOG_MESSAGE_TYPE_WARNING,
    LOG_MESSAGE_TYPE_ERROR
}log_message_type_t;

class Log : public QObject
{
    Q_OBJECT
public:
    explicit    Log(QObject *parent = nullptr);
    void        assignLogWidget(QPlainTextEdit* aWidget);
    void        printLogMessage(QString message, log_message_type_t type);

signals:

private:
    QPlainTextEdit* plainTextWidget;

};

#endif // LOG_H
