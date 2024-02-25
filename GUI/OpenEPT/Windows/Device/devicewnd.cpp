#include "devicewnd.h"
#include "ui_devicewnd.h"
#include "Windows/Console/consolewnd.h"
#include <QFileDialog>
#include <QNetworkInterface>

/*TODO: Declare this in config file*/
#define PLOT_MINIMUM_SIZE_WIDTH     600
#define PLOT_MINIMUM_SIZE_HEIGHT    200


DeviceWnd::DeviceWnd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceWnd)
{
    ui->setupUi(this);
//    if (!consoleWnd) {
//        qDebug() << "Error: Failed to allocate memory for consoleWnd.";
//        // Handle the error appropriately, e.g., return or exit
//    }
    /* Set default Value for ADC Resolution Comb*/
    /*
    resolutionOptions=(
        QStringList()
        <<""
        <<"16 Bit"
        <<"14 Bit"
        <<"12 Bit"
        <<"10 Bit"
        );
    ui->resolutionComb->addItems(resolutionOptions);
    */
    resolutionOptions = new QStringList();
    *resolutionOptions
        <<""
        <<"16 Bit"
        <<"14 Bit"
        <<"12 Bit"
        <<"10 Bit"
        ;
    ui->resolutionComb->addItems(*resolutionOptions);
    /* Set default Value for ADC Clock Div Comb*/
    clockDivOptions = new QStringList();
    *clockDivOptions
        <<""
        <<"1"
        <<"2"
        <<"4"
        <<"6"
        <<"8"
        <<"10"
        <<"12"
        <<"16"
        <<"32"
        <<"64"
        <<"128"
        ;
    ui->clockDivComb->addItems(*clockDivOptions);

    /* Set default Value for ADC Sample Time Comb*/
    sampleTimeOptions = new QStringList();
    *sampleTimeOptions
        <<""
        <<"1C5"
        <<"2C5"
        <<"8C5"
        <<"16C5"
        <<"32C5"
        <<"64C5"
        <<"387C5"
        <<"810C5"
        ;
    ui->sampleTimeComb->addItems(*sampleTimeOptions);


    networkInterfacesNames = new QStringList();
    *networkInterfacesNames << "";

    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    foreach(QNetworkInterface a, interfaces)
    {
        QList<QNetworkAddressEntry> allEntries = a.addressEntries();
        if(a.flags() & QNetworkInterface::IsUp){
            qDebug() << "Interface Name:" << a.name();
            QNetworkAddressEntry entry;
            foreach (entry, allEntries) {
                if( entry.ip().protocol() == QAbstractSocket::IPv4Protocol){
                    *networkInterfacesNames <<   "<" + a.name() + ">:" + entry.ip().toString();
                }
            }
        }
    }

    ui->streamServerInterfComb->addItems(*networkInterfacesNames);

    advanceConfigurationWnd  = new AdvanceConfigurationWnd();
    advanceConfigurationWnd->hide();
    //Prethodno se lista kreira dinamicki
    advanceConfigurationWnd->assignResolutionList(resolutionOptions);
    advanceConfigurationWnd->assignClockDivList(clockDivOptions);
    advanceConfigurationWnd->assignSampleTimeList(sampleTimeOptions);
    connect(ui->advanceOptionPusb, SIGNAL(clicked(bool)), this, SLOT(onAdvanceConfigurationButtonPressed(bool)));

    voltageChart             = new Plot(PLOT_MINIMUM_SIZE_WIDTH/2, PLOT_MINIMUM_SIZE_HEIGHT);
    voltageChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    voltageChart->setTitle("Voltage");
    voltageChart->setYLabel("[V]");
    voltageChart->setXLabel("[ms]");
    /* Make plot for consumption presentation */
    currentChart         = new Plot(PLOT_MINIMUM_SIZE_WIDTH/2, PLOT_MINIMUM_SIZE_HEIGHT);
    currentChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    currentChart->setTitle("Current");
    currentChart->setYLabel("[mA]");
    currentChart->setXLabel("[ms]");


    consumptionChart         = new Plot(PLOT_MINIMUM_SIZE_WIDTH, PLOT_MINIMUM_SIZE_HEIGHT);
    consumptionChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    consumptionChart->setTitle("Consumption");
    consumptionChart->setYLabel("[mAh]");
    consumptionChart->setXLabel("[ms]");

    setDeviceState(DEVICE_STATE_UNDEFINED);

    consoleWnd  = new ConsoleWnd();

    ui->GraphicsTopHorl->addWidget(voltageChart);
    ui->GraphicsTopHorl->addWidget(currentChart);
    ui->GraphicsBottomVerl->addWidget(consumptionChart, Qt::AlignCenter);
    setDeviceInterfaceSelectionState(DEVICE_INTERFACE_SELECTION_STATE_UNDEFINED);

    connect(ui->saveToFileCheb, SIGNAL(stateChanged(int)), this, SLOT(onSaveToFileChanged(int)));
    connect(ui->pathPusb, SIGNAL(clicked(bool)), this, SLOT(onPathInfo()));
    connect(ui->startPusb, SIGNAL(clicked(bool)), this, SLOT(onStartAcquisition()));
    connect(ui->pausePusb, SIGNAL(clicked(bool)), this, SLOT(onPauseAcquisition()));
    connect(ui->stopPusb, SIGNAL(clicked(bool)), this, SLOT(onStopAcquisiton()));
    connect(ui->refreshPusb, SIGNAL(clicked(bool)), this, SLOT(onRefreshAcquisiton()));
    connect(ui->ConsolePusb, SIGNAL(clicked(bool)), this, SLOT(onConsolePressed()));
    connect(consoleWnd, SIGNAL(sigControlMsgSend(QString)), this, SLOT(onNewControlMsgRcvd(QString)));
    connect(ui->resolutionComb, SIGNAL(currentIndexChanged(int)), this, SLOT(onResolutionChanged(int)));
    connect(ui->clockDivComb, SIGNAL(currentIndexChanged(int)), this, SLOT(onClockDivChanged(int)));
    connect(ui->sampleTimeComb, SIGNAL(currentIndexChanged(int)), this, SLOT(onSampleTimeChanged(int)));
    connect(ui->samplingTimeLine, SIGNAL(returnPressed()), this, SLOT(onSamplingTimeChanged()));
    connect(ui->samplingTimeLine, SIGNAL(textChanged(QString)), this, SLOT(onSamplingTimeTxtChanged(QString)));
    connect(ui->streamServerInterfComb, SIGNAL(currentTextChanged(QString)), this, SLOT(onInterfaceChanged(QString)));
    connect(advanceConfigurationWnd, SIGNAL(sigAdvResolutionChanged(int)), this, SLOT(onAdvResolutionChanged(int)));
    connect(advanceConfigurationWnd, SIGNAL(sigAdvClockDivChanged(int)), this, SLOT(onAdvClockDivChanged(int)));
    connect(advanceConfigurationWnd, SIGNAL(sigAdvSampleTimeChanged(int)), this, SLOT(onAdvSampleTimeChanged(int)));
    //connect(advanceConfigurationWnd, SIGNAL(sigAdvSamplingTimeChanged(QString)), this, SLOT(onAdvSamplingTimeChanged(QString)));
    connect(advanceConfigurationWnd, SIGNAL(sigAdvConfigurationChanged(QVariant)), this, SLOT(onAdvConfigurationChanged(QVariant)));
}

void    DeviceWnd::onNewControlMsgRcvd(QString text)
{
    /* emit signal to deviceContrainer -> */
    emit sigNewControlMessageRcvd(text);
}

void DeviceWnd::onPathInfo()
{
    QString chosenPath = QFileDialog::getExistingDirectory(this, "Select Directory", QDir::homePath());
    ui->pathLine->setText(chosenPath);
}

void DeviceWnd::onAdvResolutionChanged(int index)
{
    ui->resolutionComb->blockSignals(true);
    ui->resolutionComb->setCurrentIndex(index);
    ui->resolutionComb->blockSignals(false);
}

void DeviceWnd::onAdvClockDivChanged(int index)
{
    ui->clockDivComb->blockSignals(true);
    ui->clockDivComb->setCurrentIndex(index);
    ui->clockDivComb->blockSignals(false);
}

void DeviceWnd::onAdvSampleTimeChanged(int index)
{
    ui->sampleTimeComb->blockSignals(true);
    ui->sampleTimeComb->setCurrentIndex(index);
    ui->sampleTimeComb->blockSignals(false);
}

void DeviceWnd::onInterfaceChanged(QString interfaceInfo)
{
    QString ip;
    QStringList interfaceInfoParts;
    interfaceInfoParts = interfaceInfo.split(":");
    ip = interfaceInfoParts[1];
    emit sigNewInterfaceSelected(ip);
}

void DeviceWnd::onAdvSamplingTimeChanged(QString time)
{
    ui->samplingTimeLine->blockSignals(true);
    ui->samplingTimeLine->setText(time);
    ui->samplingTimeLine->blockSignals(false);
}
void DeviceWnd::onAdvConfigurationChanged(QVariant data)
{
    advConfigurationData adata = data.value<advConfigurationData>();
    ui->resolutionComb  ->  setCurrentText(adata.resolution);
    ui->sampleTimeComb  ->  setCurrentText(adata.sampleTime);
    ui->clockDivComb    ->  setCurrentText(adata.clockDiv);
    samplingTextChanged = true;
    ui->samplingTimeLine->  setText(adata.samplingTime);
    DeviceWnd::onAvrRatioChanged(adata.averaginRatioIndex);
    DeviceWnd::onVOffsetChanged(adata.voltageOffset);
    DeviceWnd::onCOffsetChanged(adata.currentOffset);

}

void DeviceWnd::onVOffsetChanged(QString off)
{
    advanceConfigurationWnd->SetVOffsetFromDevWnd();
    emit sigVOffsetChanged(off);
}

void DeviceWnd::onCOffsetChanged(QString off)
{
    advanceConfigurationWnd->SetCOffsetFromDevWnd();
    emit sigCOffsetChanged(off);
}

void DeviceWnd::onAvrRatioChanged(int index)
{
    advanceConfigurationWnd->SetAvrRatioFromDevWnd();
    //emit sigAvrRatioChanged(ui->);
}

void DeviceWnd::onResolutionChanged(int index)
{
    advanceConfigurationWnd->SetResolutionFromDevWnd(index);
    emit sigResolutionChanged(ui->resolutionComb->currentText());
}

void DeviceWnd::onClockDivChanged(int index)
{
    advanceConfigurationWnd->SetClockDivFromDevWnd(index);
    emit sigClockDivChanged(ui->clockDivComb->currentText());
}

void DeviceWnd::onSampleTimeChanged(int index)
{
    advanceConfigurationWnd->SetSampleTimeFromDevWnd(index);
    emit sigSampleTimeChanged(ui->sampleTimeComb->currentText());
}

void DeviceWnd::onSamplingTimeChanged()
{
    QString time = ui->samplingTimeLine->text();
    advanceConfigurationWnd->SetSamplingTimeFromDevWnd(time);
    emit sigSamplingTimeChanged(time);
}

void DeviceWnd::onSamplingTimeTxtChanged(QString time)
{
    if (samplingTextChanged != true)
    {
        samplingTextChanged = false;
        return;
    } else
    {
        advanceConfigurationWnd->SetSamplingTimeFromDevWnd(time);
        samplingTextChanged = false;
        emit sigSamplingTimeChanged(time);
    }
}

void    DeviceWnd::onAdvanceConfigurationButtonPressed(bool pressed)
{
    advanceConfigurationWnd->show();
}

void    DeviceWnd::onSaveToFileChanged(int value)
{
    if(value == Qt::Checked)
    {
        ui->pathPusb->setEnabled(true);
        ui->pathLab->setEnabled(true);
        ui->pathLine->setEnabled(true);
        emit saveToFileEnabled(true);
    }else
    {
        ui->pathPusb->setEnabled(false);
        ui->pathLab->setEnabled(false);
        ui->pathLine->setEnabled(false);
        emit saveToFileEnabled(false);
    }
}
/*
void DeviceWnd::onInfoSaveToFileEnabled(bool enableStatus)
{
    saveToFileFlag = enableStatus;
}
*/

void DeviceWnd::onConsolePressed()
{
    consoleWnd->show();
}

void DeviceWnd::onStartAcquisition()
{
    emit sigStartAcquisition();
}

void DeviceWnd::onPauseAcquisition()
{
    emit sigPauseAcquisition();
}

void DeviceWnd::onStopAcquisiton()
{
    emit sigStopAcquisition();
}

void DeviceWnd::onRefreshAcquisiton()
{
    emit sigRefreshAcquisition();
}

void DeviceWnd::setDeviceStateDisconnected()
{
    ui->startPusb->setEnabled(false);
    ui->stopPusb->setEnabled(false);
    ui->pausePusb->setEnabled(false);
    ui->refreshPusb->setEnabled(false);
    ui->dischargeControlPusb1->setEnabled(false);
    ui->dischargeControlPusb2->setEnabled(false);
    ui->deviceConnectedLabe->setText("Disconnected");
    ui->deviceConnectedLabe->setStyleSheet("QLabel { background-color : red; color : black; }");
}

void DeviceWnd::setDeviceStateConnected()
{
    ui->startPusb->setEnabled(true);
    ui->stopPusb->setEnabled(true);
    ui->pausePusb->setEnabled(true);
    ui->refreshPusb->setEnabled(true);
    ui->dischargeControlPusb1->setEnabled(true);
    ui->dischargeControlPusb2->setEnabled(true);
    ui->deviceConnectedLabe->setText("Connected");
    ui->deviceConnectedLabe->setStyleSheet("QLabel { background-color : green; color : black; }");
}

/*
void DeviceWnd::onClockDivCombIndexChanged(int index)
{
    emit sigClockDivChanged(clockDivOptions[index]);
}

void DeviceWnd::onResolutionCombIndexChanged(int index)
{
    emit sigResolutionChanged(resolutionOptions[index]);
}

void DeviceWnd::onSamplingTimeCombIndexChanged(int index)
{
    emit sigSamplingTimeChanged(sampleTimeOptions[index]);
}
*/
void    DeviceWnd::closeEvent(QCloseEvent *event)
{
    emit sigWndClosed();
}

DeviceWnd::~DeviceWnd()
{
    delete ui;
}

QPlainTextEdit *DeviceWnd::getLogWidget()
{
    return ui->loggingQpte;
}

void DeviceWnd::setDeviceState(device_state_t aDeviceState)
{
    deviceState = aDeviceState;
    switch(deviceState)
    {
    case DEVICE_STATE_UNDEFINED:
        setDeviceStateDisconnected();
        break;
    case DEVICE_STATE_CONNECTED:
        setDeviceStateConnected();
        break;
    case DEVICE_STATE_DISCONNECTED:
        setDeviceStateDisconnected();
        break;
    }
}

void DeviceWnd::printConsoleMsg(QString msg)
{
    /* call consoleWnd print Message to display recieved msg form FW <- */
    consoleWnd->printMessage(msg);
}

void DeviceWnd::setDeviceInterfaceSelectionState(device_interface_selection_state_t selectionState)
{
    switch(selectionState)
    {
    case DEVICE_INTERFACE_SELECTION_STATE_UNDEFINED:
        ui->samplingTimeLine->setEnabled(false);
        ui->resolutionComb->setEnabled(false);
        ui->clockDivComb->setEnabled(false);
        ui->sampleTimeComb->setEnabled(false);
        ui->advanceOptionPusb->setEnabled(false);
        ui->maxNumOfPacketsLine->setEnabled(false);
        ui->saveToFileCheb->setEnabled(false);
        ui->dischargeControlPusb1->setEnabled(false);
        ui->dischargeControlPusb2->setEnabled(false);
        ui->pathPusb->setEnabled(false);
        ui->streamServerInterfComb->setEnabled(true);
        break;
    case DEVICE_INTERFACE_SELECTION_STATE_SELECTED:
        ui->samplingTimeLine->setEnabled(true);
        ui->resolutionComb->setEnabled(true);
        ui->clockDivComb->setEnabled(true);
        ui->sampleTimeComb->setEnabled(true);
        ui->advanceOptionPusb->setEnabled(true);
        ui->maxNumOfPacketsLine->setEnabled(true);
        ui->saveToFileCheb->setEnabled(true);
        ui->dischargeControlPusb1->setEnabled(true);
        ui->dischargeControlPusb2->setEnabled(true);
        ui->pathPusb->setEnabled(true);
        ui->streamServerInterfComb->setEnabled(false);
        break;
    }
    interfaceState = selectionState;
}

QStringList *DeviceWnd::getChSamplingTimeOptions()
{
    return sampleTimeOptions;
}

QStringList *DeviceWnd::getChAvgRationOptions()
{
    return sampleTimeOptions;
}

QStringList *DeviceWnd::getClockDivOptions()
{
    return clockDivOptions;
}

QStringList *DeviceWnd::getResolutionOptions()
{
    return resolutionOptions;
}

