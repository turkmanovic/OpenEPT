#include <QSpacerItem>
#include "advanceconfigurationwnd.h"
#include "ui_advanceconfigurationwnd.h"

#define LINEDIT_WIDTH   50
#define LABEL_WIDTH     220

AdvanceConfigurationWnd::AdvanceConfigurationWnd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdvanceConfigurationWnd)
{
    ui->setupUi(this);

    QStringList selectProfileOptions=(
        QStringList()<<
                        ""<<
                        "Static"<<
                        "Ramp"
                        );
    ui->selectProfileComb->addItems(selectProfileOptions);
    connect(ui->selectProfileComb, SIGNAL(currentIndexChanged(int)), this, SLOT(onLoadProfileChanged(int)));


    /* Uknown profile layout */
    QLabel*  uknownLabel = new QLabel();
    uknownLabel->setText("Select profile");
    uknownLabel->setAlignment(Qt::AlignCenter);
    uknownLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    uknownProfileLayout = new QHBoxLayout();
    uknownProfileLayout->addWidget(uknownLabel);

    uknownProfileWidg   = new QWidget();
    uknownProfileWidg->setLayout(uknownProfileLayout);

    /* Static profile layout */
    /**/
    QLabel*  averageCurrentLabel = new QLabel();
    averageCurrentLabel->setText("Average current [mA]:");
    averageCurrentLabel->setAlignment(Qt::AlignLeft);
    averageCurrentLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    averageCurrentLabel->setFixedWidth(LABEL_WIDTH);

    staticProfileAverageCurrentLine  = new QLineEdit();
    staticProfileAverageCurrentLine->setFixedWidth(LINEDIT_WIDTH);
    staticProfileAverageCurrentLine->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    staticProfileAverageCurrentLine->setText(QString::number(100));

    QLabel*  averageCurrentDevLabel = new QLabel();
    averageCurrentDevLabel->setText("Average current dev [%]:");
    averageCurrentDevLabel->setAlignment(Qt::AlignLeft);
    averageCurrentDevLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    averageCurrentDevLabel->setFixedWidth(LABEL_WIDTH);

    staticProfileAverageCurrentDeviationLine = new QLineEdit();
    staticProfileAverageCurrentDeviationLine->setFixedWidth(LINEDIT_WIDTH);
    staticProfileAverageCurrentDeviationLine->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    staticProfileAverageCurrentDeviationLine->setText(QString::number(10));



    QHBoxLayout *averageCurrentHorl = new QHBoxLayout();
    averageCurrentHorl->addWidget(averageCurrentLabel);
    averageCurrentHorl->addWidget(staticProfileAverageCurrentLine);
    averageCurrentHorl->addWidget(averageCurrentDevLabel);
    averageCurrentHorl->addWidget(staticProfileAverageCurrentDeviationLine);
    averageCurrentHorl->addStretch();


    /**/
    QLabel*  averageCurrentChangePeriodLabe = new QLabel();
    averageCurrentChangePeriodLabe->setText("Average current change period [ms]:");
    averageCurrentChangePeriodLabe->setAlignment(Qt::AlignLeft);
    averageCurrentChangePeriodLabe->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    averageCurrentChangePeriodLabe->setFixedWidth(LABEL_WIDTH);

    staticProfileAverageCurrentChangePeriodLine  = new QLineEdit();
    staticProfileAverageCurrentChangePeriodLine->setFixedWidth(LINEDIT_WIDTH);
    staticProfileAverageCurrentChangePeriodLine->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    staticProfileAverageCurrentChangePeriodLine->setText(QString::number(100));

    QLabel*  AverageCurrentDeviationLabe = new QLabel();
    AverageCurrentDeviationLabe->setText("Average current change period dev [%]:");
    AverageCurrentDeviationLabe->setAlignment(Qt::AlignLeft);
    AverageCurrentDeviationLabe->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    AverageCurrentDeviationLabe->setFixedWidth(LABEL_WIDTH);

    staticProfileAverageCurrentDeviationLine = new QLineEdit();
    staticProfileAverageCurrentDeviationLine->setFixedWidth(LINEDIT_WIDTH);
    staticProfileAverageCurrentDeviationLine->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    staticProfileAverageCurrentDeviationLine->setText(QString::number(10));


    QHBoxLayout *averageCurrentChangePeriodHorl = new QHBoxLayout();
    averageCurrentChangePeriodHorl->addWidget(averageCurrentChangePeriodLabe);
    averageCurrentChangePeriodHorl->addWidget(staticProfileAverageCurrentChangePeriodLine);
    averageCurrentChangePeriodHorl->addWidget(AverageCurrentDeviationLabe);
    averageCurrentChangePeriodHorl->addWidget(staticProfileAverageCurrentDeviationLine);
    averageCurrentChangePeriodHorl->addStretch();

    /**/
    QLabel*  staticProfileDurationLabe = new QLabel();
    staticProfileDurationLabe->setText("Profile duration [ms]:");
    staticProfileDurationLabe->setAlignment(Qt::AlignLeft);
    staticProfileDurationLabe->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    staticProfileDurationLabe->setFixedWidth(LABEL_WIDTH);

    staticProfileDurationLine  = new QLineEdit();
    staticProfileDurationLine->setFixedWidth(LINEDIT_WIDTH);
    staticProfileDurationLine->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    staticProfileDurationLine->setText(QString::number(100));

    QLabel*  staticProfileDurationDevLabe = new QLabel();
    staticProfileDurationDevLabe->setText("Profile duration dev [%]:");
    staticProfileDurationDevLabe->setAlignment(Qt::AlignLeft);
    staticProfileDurationDevLabe->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    staticProfileDurationDevLabe->setFixedWidth(LABEL_WIDTH);

    staticProfileDurationDevLine = new QLineEdit();
    staticProfileDurationDevLine->setFixedWidth(LINEDIT_WIDTH);
    staticProfileDurationDevLine->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    staticProfileDurationDevLine->setText(QString::number(10));


    QHBoxLayout *staticProfileDurationHorl = new QHBoxLayout();
    staticProfileDurationHorl->addWidget(staticProfileDurationLabe);
    staticProfileDurationHorl->addWidget(staticProfileDurationLine);
    staticProfileDurationHorl->addWidget(staticProfileDurationDevLabe);
    staticProfileDurationHorl->addWidget(staticProfileDurationDevLine);
    staticProfileDurationHorl->addStretch();

    staticProfileLayout = new QVBoxLayout();
    staticProfileLayout->addLayout(averageCurrentHorl);
    staticProfileLayout->addLayout(averageCurrentChangePeriodHorl);
    staticProfileLayout->addLayout(staticProfileDurationHorl);

    staticProfileWidg   = new QWidget();
    staticProfileWidg->setLayout(staticProfileLayout);

    SetProfileTypeUknown();
    activeLoadProfileType = LOAD_PROFILE_TYPE_UNKNOW;
}


void    AdvanceConfigurationWnd::SetProfileTypeStatic()
{
    staticProfileWidg->show();
    ui->SelectprofileVerlProfileconfigurationHorl->addWidget(staticProfileWidg);
}

void    AdvanceConfigurationWnd::RemoveProfileTypeStatic()
{
    staticProfileWidg->hide();
    ui->SelectprofileVerlProfileconfigurationHorl->removeWidget(staticProfileWidg);
}

void    AdvanceConfigurationWnd::SetProfileTypeRamp()
{

}
void    AdvanceConfigurationWnd::RemoveProfileTypeRamp()
{

}

void    AdvanceConfigurationWnd::SetProfileTypeUknown()
{
    uknownProfileWidg->show();
    ui->SelectprofileVerlProfileconfigurationHorl->addWidget(uknownProfileWidg);
}
void    AdvanceConfigurationWnd::RemoveProfileTypeUknown()
{
    uknownProfileWidg->hide();
    ui->SelectprofileVerlProfileconfigurationHorl->removeWidget(uknownProfileWidg);
}
void    AdvanceConfigurationWnd::RemoveProfileType(loadProfileType_t aProfileType)
{
    switch(aProfileType)
    {
    case LOAD_PROFILE_TYPE_UNKNOW:
        RemoveProfileTypeUknown();
        break;
    case LOAD_PROFILE_TYPE_STATIC:
        RemoveProfileTypeStatic();
        break;
    case LOAD_PROFILE_TYPE_RAMP:
        RemoveProfileTypeRamp();
        break;
    }
    activeLoadProfileType = aProfileType;
}
void    AdvanceConfigurationWnd::SetProfileType(loadProfileType_t aProfileType)
{
    switch(aProfileType)
    {
    case LOAD_PROFILE_TYPE_UNKNOW:
        SetProfileTypeUknown();
        break;
    case LOAD_PROFILE_TYPE_STATIC:
        SetProfileTypeStatic();
        break;
    case LOAD_PROFILE_TYPE_RAMP:
        SetProfileTypeRamp();
        break;
    }
    activeLoadProfileType = aProfileType;
}

void    AdvanceConfigurationWnd::onLoadProfileChanged(int loadIndex)
{
    RemoveProfileType(activeLoadProfileType);
    switch(loadIndex)
    {
    case 0:
        SetProfileType(LOAD_PROFILE_TYPE_UNKNOW);
        break;
    case 1:
        SetProfileType(LOAD_PROFILE_TYPE_STATIC);
        break;
    case 2:
        SetProfileType(LOAD_PROFILE_TYPE_RAMP);
        break;
    default:
        break;
    }
}
AdvanceConfigurationWnd::~AdvanceConfigurationWnd()
{
    delete ui;
}
