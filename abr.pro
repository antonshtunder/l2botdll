#-------------------------------------------------
#
# Project created by QtCreator 2018-02-17T22:11:47
#
#-------------------------------------------------

QT       -= gui

TARGET = abr
TEMPLATE = lib

DEFINES += ABR_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        abr.cpp \
    lineage/lineageglobal.cpp \
    lineage/player.cpp \
    utils.cpp \
    lineage/mob.cpp \
    lineage/instance.cpp \
    lineage/droppeditem.cpp \
    lineage/inventory.cpp \
    lineage/skilllist.cpp \
    lineage/skill.cpp \
    lineage/inventoryitem.cpp \
    lineage/effectlist.cpp \
    ipc.cpp \
    synchronization.cpp \
    lineage/l2utils/macro.cpp \
    lineage/effect.cpp \
    patches.cpp \
    lineage/instances/item.cpp \
    winfuncs.cpp

HEADERS += \
        abr.h \
    lineage/lineageglobal.h \
    lineage/player.h \
    utils.h \
    lineage/mob.h \
    lineage/instance.h \
    lineage/droppeditem.h \
    lineage/inventory.h \
    lineage/skilllist.h \
    lineage/skill.h \
    lineage/inventoryitem.h \
    lineage/effectlist.h \
    ipc.h \
    synchronization.h \
    lineage/l2utils/macro.h \
    lineage/effect.h \
    patches.h \
    lineage/instances/item.h \
    winfuncs.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Libs/build-DllHackLibrary-Desktop_Qt_5_9_2_MSVC2015_32bit-Debug/release/ -lDllHackLibrary
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Libs/build-DllHackLibrary-Desktop_Qt_5_9_2_MSVC2015_32bit-Debug/debug/ -lDllHackLibrary

INCLUDEPATH += $$PWD/../../Libs/DllHackLibrary
DEPENDPATH += $$PWD/../../Libs/DllHackLibrary

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Libs/build-DllHackLibrary-Desktop_Qt_5_9_2_MSVC2015_32bit-Debug/release/libDllHackLibrary.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Libs/build-DllHackLibrary-Desktop_Qt_5_9_2_MSVC2015_32bit-Debug/debug/libDllHackLibrary.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Libs/build-DllHackLibrary-Desktop_Qt_5_9_2_MSVC2015_32bit-Debug/release/DllHackLibrary.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Libs/build-DllHackLibrary-Desktop_Qt_5_9_2_MSVC2015_32bit-Debug/debug/DllHackLibrary.lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Libs/build-lineageIPC-Desktop_Qt_5_9_2_MSVC2015_32bit-Debug/release/ -llineageIPC
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Libs/build-lineageIPC-Desktop_Qt_5_9_2_MSVC2015_32bit-Debug/debug/ -llineageIPC

INCLUDEPATH += $$PWD/../../Libs/lineageIPC
DEPENDPATH += $$PWD/../../Libs/lineageIPC

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Libs/build-lineageIPC-Desktop_Qt_5_9_2_MSVC2015_32bit-Debug/release/liblineageIPC.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Libs/build-lineageIPC-Desktop_Qt_5_9_2_MSVC2015_32bit-Debug/debug/liblineageIPC.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Libs/build-lineageIPC-Desktop_Qt_5_9_2_MSVC2015_32bit-Debug/release/lineageIPC.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Libs/build-lineageIPC-Desktop_Qt_5_9_2_MSVC2015_32bit-Debug/debug/lineageIPC.lib
