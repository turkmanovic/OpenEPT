QT       += core gui uitools
QT       += network
LIBS     += -lws2_32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Chart/qcustomplot.cpp \
    Links/controllink.cpp \
    Links/statuslink.cpp \
    Utility/log.cpp \
    Windows/AddDevice/adddevicewnd.cpp \
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
    Links/statuslink.h \
    Utility/log.h \
    Windows/AddDevice/adddevicewnd.h \
    Windows/Device/advanceconfigurationwnd.h \
    Windows/Device/devicewnd.h \
    Windows/Plot/plot.h \
    device.h \
    devicecontainer.h \
    openept.h

FORMS += \
    Windows/AddDevice/adddevicewnd.ui \
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
