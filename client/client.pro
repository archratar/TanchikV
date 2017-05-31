#-------------------------------------------------
#
# Project created by QtCreator 2017-03-30T23:36:53
#
#-------------------------------------------------

QT       += core gui network

QMAKE_LFLAGS += -static-libgcc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        client.cpp

HEADERS  += client.h

FORMS    += client.ui
