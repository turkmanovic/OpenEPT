#include "log.h"
#include <QTime>

Log::Log(QObject *parent)
    : QObject{parent}
{

}

void Log::assignLogWidget(QPlainTextEdit *aWidget)
{
    plainTextWidget = aWidget;
    plainTextWidget->setReadOnly(true);
}

void Log::printLogMessage(QString message, log_message_type_t type)
{
    QString tmpMessage;
    plainTextWidget->moveCursor(QTextCursor::End);
    switch(type)
    {
    case LOG_MESSAGE_TYPE_INFO:
        tmpMessage = "<p style=\"color:black;\">";
        tmpMessage += QTime::currentTime().toString() + ":";
        tmpMessage += message;
        tmpMessage += "</p>";
        plainTextWidget->appendHtml(tmpMessage);
        break;
    case LOG_MESSAGE_TYPE_WARNING:
        tmpMessage = "<p style=\"color:blue;\">";
        tmpMessage += QTime::currentTime().toString() + ":";
        tmpMessage += "[Warning]";
        tmpMessage += message;
        tmpMessage += "</p>";
        plainTextWidget->appendHtml(tmpMessage);
        break;
    case LOG_MESSAGE_TYPE_ERROR:
        tmpMessage = "<p style=\"color:red;\">";
        tmpMessage += QTime::currentTime().toString() + ":";
        tmpMessage += "[Error]";
        tmpMessage += message;
        tmpMessage += "</p>";
        plainTextWidget->appendHtml(tmpMessage);
        break;
    }
    plainTextWidget->moveCursor(QTextCursor::End);
}
