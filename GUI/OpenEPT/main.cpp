#include "openept.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenEPT w;
    w.show();
    return a.exec();
}
