#-------------------------------------------------
#
# Project created by QtCreator 2020-04-13T20:17:04
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = open_vent
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    portcomms.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    portcomms.h \
    qcustomplot.h

FORMS    += mainwindow.ui

