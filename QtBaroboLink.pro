#-------------------------------------------------
#
# Project created by QtCreator 2013-12-02T12:02:40
#
#-------------------------------------------------

QT       += core gui network

CONFIG += qt debug

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtBaroboLink
TEMPLATE = app

INCLUDEPATH = ../libbarobo/include\
              ../libbarobo/BaroboConfigFile\
              ../libbarobo/BaroboConfigFile/mxml-2.7\
              inc

SOURCES += src/main.cpp\
        src/mainwindow.cpp\
        src/robotmanager.cpp\
        src/recordmobot.cpp\
        src/configfile.cpp\
        src/qtrobotmanager.cpp\
        src/controlpanel.cpp\
        src/connectdialog.cpp\
        src/asyncrobot.cpp\
        src/scandialog.cpp\
        src/scanlist.cpp\
        src/tcpcomms.cpp\
        src/QMobot.cpp

HEADERS  += inc/mainwindow.h\
            inc/robotmanager.h\
            inc/recordmobot.h\
            inc/configfile.h\
            inc/qtrobotmanager.h\
            inc/controlpanel.h\
            inc/connectdialog.h\
            inc/asyncrobot.h\
            inc/scandialog.h\
            inc/scanlist.h\
            inc/tcpcomms.h\
            inc/QMobot.h

FORMS    += mainwindow.ui \
    controlpanel.ui \
    connectpanel.ui \
    scandialog.ui \
    scanwidget.ui

LIBS += -L../stage/lib -lbarobo -lsfp

RESOURCES += \
    resource.qrc
