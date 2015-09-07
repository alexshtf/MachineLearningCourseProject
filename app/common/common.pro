#-------------------------------------------------
#
# Project created by QtCreator 2015-07-26T13:18:34
#
#-------------------------------------------------

include(../common.pri)

QT       -= core gui

TARGET = common
TEMPLATE = lib

CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += PixelsLabelsArray.BIF.cpp

HEADERS += PixelsLabelsArray.h \
    PixelsLabelsArray.BIF.h \
    util.h

# boost
INCLUDEPATH += $$PWD/../boost-libs/include
