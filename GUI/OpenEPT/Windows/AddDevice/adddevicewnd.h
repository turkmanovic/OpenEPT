#ifndef ADDDEVICEWND_H
#define ADDDEVICEWND_H

#include <QWidget>

namespace Ui {
class adddevicewnd;
}

class adddevicewnd : public QWidget
{
    Q_OBJECT

public:
    explicit adddevicewnd(QWidget *parent = nullptr);
    ~adddevicewnd();

private:
    Ui::adddevicewnd *ui;
};

#endif // ADDDEVICEWND_H
