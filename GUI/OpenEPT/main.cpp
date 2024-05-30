#include "openept.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    // Set up OpenGL format
    QSurfaceFormat format;
    format.setSamples(4); // Enable 4x multisampling for anti-aliasing
    QSurfaceFormat::setDefaultFormat(format);
    OpenEPT w;
    w.show();
    return a.exec();
}
