#include "devicewnd.h"
#include "ui_devicewnd.h"
#include "Windows/Console/consolewnd.h"
#include <QFileDialog>

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
    connect(advanceConfigurationWnd, SIGNAL(sigAdvResolutionChanged(int)), this, SLOT(onAdvResolutionChanged(int)));
    connect(advanceConfigurationWnd, SIGNAL(sigAdvClockDivChanged(int)), this, SLOT(onAdvClockDivChanged(int)));
    connect(advanceConfigurationWnd, SIGNAL(sigAdvSampleTimeChanged(int)), this, SLOT(onAdvSampleTimeChanged(int)));
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

void DeviceWnd::onAdvConfigurationChanged(QVariant data)
{
    advConfigurationData adata = data.value<advConfigurationData>();
    ui->resolutionComb  ->  setCurrentText(adata.resolution);
    ui->sampleTimeComb  ->  setCurrentText(adata.sampleTime);
    ui->clockDivComb    ->  setCurrentText(adata.clockDiv);
}

void DeviceWnd::onResolutionChanged(int index)
{
    advanceConfigurationWnd->SetResolutionFromDevWnd(index);
    emit sigResolutionChanged(index);
}

void DeviceWnd::onClockDivChanged(int index)
{
    advanceConfigurationWnd->SetClockDivFromDevWnd(index);
    emit sigClockDivChanged(index);
}

void DeviceWnd::onSampleTimeChanged(int index)
{
    advanceConfigurationWnd->SetSampleTimeFromDevWnd(index);
    emit sigSampleTimeChanged(index);
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
    emit startAcquisition();
}

void DeviceWnd::onPauseAcquisition()
{
    emit pauseAcquisition();
}

void DeviceWnd::onStopAcquisiton()
{
    emit stopAcquisition();
}

void DeviceWnd::onRefreshAcquisiton()
{
    emit refreshAcquisition();
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
