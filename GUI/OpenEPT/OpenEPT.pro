QT       += core gui uitools opengl
QT       += network
LIBS     += -lws2_32
#LIBS     += -lws2_32 -lOpenGL32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17
#DEFINES += QCUSTOMPLOT_USE_OPENGL
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Chart/qcustomplot.cpp \
    Links/controllink.cpp \
    Links/eplink.cpp \
    Links/statuslink.cpp \
    Links/streamlink.cpp \
    Processing/dataprocessing.cpp \
    Processing/epprocessing.cpp \
    Processing/fileprocessing.cpp \
    Utility/log.cpp \
    Windows/AddDevice/adddevicewnd.cpp \
    Windows/Console/consolewnd.cpp \
    Windows/Device/advanceconfigurationwnd.cpp \
    Windows/Device/devicewnd.cpp \
    Windows/Plot/plot.cpp \
    device.cpp \
    devicecontainer.cpp \
    main.cpp \
    openept.cpp

HEADERS += \
    Chart/qcustomplot.h \
    Links/controllink.h \
    Links/eplink.h \
    Links/statuslink.h \
    Links/streamlink.h \
    Processing/dataprocessing.h \
    Processing/epprocessing.h \
    Processing/fileprocessing.h \
    Utility/log.h \
    Windows/AddDevice/adddevicewnd.h \
    Windows/Console/consolewnd.h \
    Windows/Device/advanceconfigurationwnd.h \
    Windows/Device/devicewnd.h \
    Windows/Plot/plot.h \
    device.h \
    devicecontainer.h \
    openept.h

FORMS += \
    Windows/AddDevice/adddevicewnd.ui \
    Windows/Console/consolewnd.ui \
    Windows/Device/advanceconfigurationwnd.ui \
    Windows/Device/devicewnd.ui \
    openept.ui

QTPLUGIN += qjpeg
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
