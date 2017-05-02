QT += core serialport
QT -= gui

CONFIG += c++11

TARGET = Askcpuload
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
           qhardwarestatusexplorer.cpp \
    qhardwarestatusservice.cpp

HEADERS += \
           qhardwarestatusexplorer.h \
    qhardwarestatusservice.h

include($${PWD}/qtservice/common.pri)
include($${PWD}/qtservice/src/qtservice.pri)
