#ifndef CONSOLEWND_H
#define CONSOLEWND_H

#include <QWidget>
#include "Utility/log.h"

namespace Ui {
class ConsoleWnd;
}

class ConsoleWnd : public QWidget
{
    Q_OBJECT

public:
    explicit ConsoleWnd(QWidget *parent = nullptr);
    ~ConsoleWnd();
    void     printMessage(QString msg);


signals:
    void onHelloSend(const QString &text);

private slots:
    void onSendClicked();
    void onOkRecieved();
private:
    Ui::ConsoleWnd *ui;
    Log             logUtil;

};

#endif // CONSOLEWND_H
