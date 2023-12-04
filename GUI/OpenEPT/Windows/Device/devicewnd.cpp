#include "devicewnd.h"
#include "ui_devicewnd.h"

/*TODO: Declare this in config file*/
#define PLOT_MINIMUM_SIZE_WIDTH     600
#define PLOT_MINIMUM_SIZE_HEIGHT    200


DeviceWnd::DeviceWnd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceWnd)
{
    ui->setupUi(this);

    /* Set default Value for ADC Resolution Comb*/
    QStringList resolutionOptions=(
        QStringList()
        <<""
        <<"16 Bit"
        <<"14 Bit"
        <<"12 Bit"
        <<"10 Bit"
        );
    ui->resolutionComb->addItems(resolutionOptions);

    /* Set default Value for ADC Clock Div Comb*/
    QStringList clockDivOptions=(
        QStringList()
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
        );
    ui->clockDivComb->addItems(clockDivOptions);

    /* Set default Value for ADC Sample Time Comb*/
    QStringList sampleTimeOptions=(
        QStringList()
        <<""
        <<"1C5"
        <<"2C5"
        <<"8C5"
        <<"16C5"
        <<"32C5"
        <<"64C5"
        <<"387C5"
        <<"810C5"
        );
    ui->sampleTimeComb->addItems(sampleTimeOptions);

    advanceConfigurationWnd  = new AdvanceConfigurationWnd();
    advanceConfigurationWnd->hide();
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

    ui->GraphicsTopHorl->addWidget(voltageChart);
    ui->GraphicsTopHorl->addWidget(currentChart);
    ui->GraphicsBottomVerl->addWidget(consumptionChart, Qt::AlignCenter);
}

void    DeviceWnd::onAdvanceConfigurationButtonPressed(bool pressed)
{
    advanceConfigurationWnd->show();
}

void    DeviceWnd::closeEvent(QCloseEvent *event)
{
    emit sigWndClosed();
}

DeviceWnd::~DeviceWnd()
{
    delete ui;
}
