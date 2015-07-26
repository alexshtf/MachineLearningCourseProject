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


# libsvm

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libsvm/release/ -llibsvm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libsvm/debug/ -llibsvm
else:unix: LIBS += -L$$OUT_PWD/../libsvm/ -llibsvm

INCLUDEPATH += $$PWD/../libsvm
DEPENDPATH += $$PWD/../libsvm

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libsvm/release/liblibsvm.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libsvm/debug/liblibsvm.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libsvm/release/libsvm.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libsvm/debug/libsvm.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libsvm/liblibsvm.a

# common

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../common/release/ -lcommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../common/debug/ -lcommon
else:unix: LIBS += -L$$OUT_PWD/../common/ -lcommon

INCLUDEPATH += $$PWD/../common
DEPENDPATH += $$PWD/../common

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../common/release/libcommon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../common/debug/libcommon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../common/release/common.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../common/debug/common.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../common/libcommon.a
