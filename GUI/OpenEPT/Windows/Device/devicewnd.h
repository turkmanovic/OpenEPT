#ifndef DEVICEWND_H
#define DEVICEWND_H

#include <QWidget>
#include "Windows/Plot/plot.h"
#include "Windows/Device/advanceconfigurationwnd.h"

namespace Ui {
class DeviceWnd;
}

class DeviceWnd : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceWnd(QWidget *parent = nullptr);
    ~DeviceWnd();

public slots:
    void    onAdvanceConfigurationButtonPressed(bool pressed);

private:
    Ui::DeviceWnd *ui;

    AdvanceConfigurationWnd* advanceConfigurationWnd;

    Plot    *voltageChart;
    Plot    *currentChart;
    Plot    *consumptionChart;
};

#endif // DEVICEWND_H
