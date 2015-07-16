#-------------------------------------------------
#
# Project created by QtCreator 2015-07-16T10:36:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ml_project
TEMPLATE = app
CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    zoommediator.cpp \
    scribblemediator.cpp

HEADERS  += mainwindow.h \
    zoommediator.h \
    scribblemediator.h

FORMS    += mainwindow.ui
