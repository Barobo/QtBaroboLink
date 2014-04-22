#-------------------------------------------------
#
# Project created by QtCreator 2013-12-02T12:02:40
#
#-------------------------------------------------

VERSION = 2.0.7

QT       += core gui network

CONFIG += qt debug

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtBaroboLink
TEMPLATE = app

INCLUDEPATH = ../stage/include \
              inc

ICON = Barobo_square.icns

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
        src/QMobot.cpp\
        src/auxcontrolpanel.cpp

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
            inc/QMobot.h\
            inc/auxcontrolpanel.h

FORMS    += mainwindow.ui \
    controlpanel.ui \
    connectpanel.ui \
    scandialog.ui \
    scanwidget.ui \
    auxcontrol.ui

LIBS += -L../stage/lib -lbarobo 

RESOURCES += \
    resource.qrc
