TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += ltcg
CONFIG += c++14

SOURCES += main.cpp \
    gridmrf.spec.cpp \
    mrfmap.spec.cpp

INCLUDEPATH += $$PWD/../catch
INCLUDEPATH += $$PWD/../boost-libs/include

# mrf

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../mrf/release/ -lmrf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../mrf/debug/ -lmrf
else:unix: LIBS += -L$$OUT_PWD/../mrf/ -lmrf

INCLUDEPATH += $$PWD/../mrf
DEPENDPATH += $$PWD/../mrf

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../mrf/release/libmrf.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../mrf/debug/libmrf.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../mrf/release/mrf.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../mrf/debug/mrf.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../mrf/libmrf.a

#common

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
