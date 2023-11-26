#ifndef OPENEPT_H
#define OPENEPT_H

#include <QMainWindow>
#include "Windows/AddDevice/adddevicewnd.h"

QT_BEGIN_NAMESPACE
namespace Ui { class OpenEPT; }
QT_END_NAMESPACE

class OpenEPT : public QMainWindow
{
    Q_OBJECT

public:
    OpenEPT(QWidget *parent = nullptr);
    ~OpenEPT();


private slots:
    void onActionAddSingleDeviceTriggered();
    void onAddDeviceWndAddDevice(QString aIpAddress, QString aPort);

private:
    Ui::OpenEPT *ui;
    //QMdiArea *mdiArea;
    AddDeviceWnd *addDeviceWnd;
};
#endif // OPENEPT_H
