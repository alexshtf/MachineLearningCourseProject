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
    scribblemediator.cpp \
    scribblemaskgenerator.cpp \
    segmentationengine.cpp

HEADERS  += mainwindow.h \
    zoommediator.h \
    scribblemediator.h \
    scribblemaskgenerator.h \
    segmentationengine.h

FORMS    += mainwindow.ui
