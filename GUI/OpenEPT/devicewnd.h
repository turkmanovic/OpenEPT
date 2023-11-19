#ifndef DEVICEWND_H
#define DEVICEWND_H

#include <QWidget>
#include "Windows/Plot/plot.h"

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

    Plot    *voltageChart;
    Plot    *currentChart;
    Plot    *consumptionChart;
};

#endif // DEVICEWND_H
