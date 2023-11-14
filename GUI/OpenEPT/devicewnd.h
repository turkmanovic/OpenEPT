#ifndef DEVICEWND_H
#define DEVICEWND_H

#include <QWidget>

namespace Ui {
class DeviceWnd;
}

class DeviceWnd : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceWnd(QWidget *parent = nullptr);
    ~DeviceWnd();

private:
    Ui::DeviceWnd *ui;
};

#endif // DEVICEWND_H
