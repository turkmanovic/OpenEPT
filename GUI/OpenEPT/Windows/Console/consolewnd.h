#ifndef CONSOLEWND_H
#define CONSOLEWND_H

#include <QWidget>

namespace Ui {
class ConsoleWnd;
}

class ConsoleWnd : public QWidget
{
    Q_OBJECT

public:
    explicit ConsoleWnd(QWidget *parent = nullptr);
    ~ConsoleWnd();

private slots:
    void onSendClicked();
private:
    Ui::ConsoleWnd *ui;
};

#endif // CONSOLEWND_H
