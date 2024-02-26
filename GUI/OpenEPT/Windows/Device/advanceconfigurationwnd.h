#ifndef ADVANCECONFIGURATIONWND_H
#define ADVANCECONFIGURATIONWND_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Windows/Device/advcofigurationdata.h"

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
    void     SetClockDivFromDevWnd(int index);
    void     SetSampleTimeFromDevWnd(int index);
    void     SetAvrRatioFromDevWnd();
    void     SetVOffsetFromDevWnd();
    void     SetCOffsetFromDevWnd();
    void     SetSamplingTimeFromDevWnd(QString time);
    void     SetInClkFromDevWnd(QString inClk);
    void     SetCoffsetFromDevWnd(QString coffset);
    void     SetVoffsetFromDevWnd(QString voffset);
    void     assignResolutionList(const QStringList *items);
    void     assignClockDivList(const QStringList *items);
    void     assignSampleTimeList(const QStringList *items);
    advConfigurationData cdata;

signals:
    void    sigAdvResolutionChanged(int resolution);
    void    sigAdvClockDivChanged(int clkDiv);
    void    sigAdvSampleTimeChanged(int sampleTime);
    void    sigAdvSamplingTimeChanged(QString time);
    void    sigAdvConfigurationChanged(QVariant data);
    void    sigAdvConfigurationRequested();
public slots:
    void    onLoadProfileChanged(int loadIndex);
    void    onAdvResolutionChanged(int index);
    void    onAdvClkDivChanged(int index);
    void    onAdvAvrRatioChanged(int index);
    void    onAdvSampleTimeChanged(int index);
    void    onAdvSamplingTimeChanged(QString time);
    void    onAdvVOffsetChanged(QString off);
    void    onAdvCOffsetChanged(QString off);
    void    onAdvConfigurationPressed(void);
    void    onAdvConfigurationRequsted(void);
private:
    Ui::AdvanceConfigurationWnd *ui;

    void    SetProfileTypeUknown();
    void    RemoveProfileTypeUknown();
    void    SetProfileTypeStatic();
    void    RemoveProfileTypeStatic();
    void    SetProfileTypeRamp();
    void    RemoveProfileTypeRamp();
    void    LabelChangedNotUpdated();
    void    LabelChangedUpdated();
    void    SetTextRed(QComboBox* cb, QLineEdit* le1, QLineEdit* le2, QLineEdit* le3);

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

    QStringList         advSampleTimeOptions;
    QStringList         advResolutionOptions;
    QStringList         advClockDivOptions;

    QString             oldVOffset = "105";
    QString             oldCOffset = "105";
    QString             oldAvrratio = "2";
};

#endif // ADVANCECONFIGURATIONWND_H
