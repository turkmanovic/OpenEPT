#ifndef ADVANCECONFIGURATIONWND_H
#define ADVANCECONFIGURATIONWND_H

#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace Ui {
class AdvanceConfigurationWnd;
enum    loadProfileType_t;
}

typedef enum loadProfileType_t
{
    LOAD_PROFILE_TYPE_UNKNOW,
    LOAD_PROFILE_TYPE_STATIC,
    LOAD_PROFILE_TYPE_RAMP
}loadProfileType_t;

class AdvanceConfigurationWnd : public QWidget
{
    Q_OBJECT

public:
    explicit AdvanceConfigurationWnd(QWidget *parent = nullptr);
    void     SetProfileType(loadProfileType_t aProfileType);
    void     RemoveProfileType(loadProfileType_t aProfileType);
    ~AdvanceConfigurationWnd();
    void     SetResolutionFromDevWnd(int index);

signals:
    void    sigAdvResolutionChanged(int resolution);
public slots:
    void    onLoadProfileChanged(int loadIndex);
    void    onAdvResolutionChanged(int index);

private:
    Ui::AdvanceConfigurationWnd *ui;

    void    SetProfileTypeUknown();
    void    RemoveProfileTypeUknown();
    void    SetProfileTypeStatic();
    void    RemoveProfileTypeStatic();
    void    SetProfileTypeRamp();
    void    RemoveProfileTypeRamp();


    QHBoxLayout*        uknownProfileLayout;
    QWidget*            uknownProfileWidg;

    QLineEdit*          staticProfileAverageCurrentLine;
    QLineEdit*          staticProfileAverageCurrentDeviationLine;
    QLineEdit*          staticProfileAverageCurrentChangePeriodLine;
    QLineEdit*          staticProfileAverageCurrentChangePeriodDevLine;
    QLineEdit*          staticProfileDurationLine;
    QLineEdit*          staticProfileDurationDevLine;
    QVBoxLayout*        staticProfileLayout;
    QWidget*            staticProfileWidg;

    loadProfileType_t   activeLoadProfileType;

    QStringList                 advSampleTimeOptions;
    QStringList                 advResolutionOptions;
    QStringList                 advClockDivOptions;
};

#endif // ADVANCECONFIGURATIONWND_H
