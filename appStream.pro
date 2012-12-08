#-------------------------------------------------
#
# Project created by QtCreator 2012-11-22T20:11:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = appStream
TEMPLATE = app
CONFIG += debug


SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp \
    audiosystem.cpp \
    serverconnectiondialog.cpp \
    streaminfodialog.cpp \
    dsp.cpp \
    processor.cpp \
    meterprocessor.cpp \
    meterwidget.cpp \
    output.cpp \
    encoder.cpp \
    encoderlame.cpp \
    outputfile.cpp \
    outputicecast.cpp \
    logger.cpp \
    statuswidget.cpp \
    encodervorbis.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h \
    audiosystem.h \
    serverconnectiondialog.h \
    streaminfodialog.h \
    ringbuffer.h \
    dsp.h \
    processor.h \
    meterprocessor.h \
    meterwidget.h \
    config.h \
    output.h \
    encoder.h \
    encoderlame.h \
    outputfile.h \
    outputicecast.h \
    logger.h \
    statuswidget.h \
    encodervorbis.h

FORMS    += mainwindow.ui \
    settingsdialog.ui \
    serverconnectiondialog.ui \
    streaminfodialog.ui

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/dep/lib/release -lportaudio_x86
    LIBS += -L$$PWD/dep/lib/release -llibmp3lame
    LIBS += -L$$PWD/dep/lib/release -llibshout    
    LIBS += -L"D:/Windows Kits/8.0/Lib/win8/um/x86/" -lWS2_32
}
win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/dep/lib/debug -lportaudio_x86
    LIBS += -llibmp3lame
    LIBS += -llibshout
    LIBS += -llibogg_static
    LIBS += -llibvorbis_static
    LIBS += -lpthreadVC2
    LIBS += -lpthreadVCE2
    LIBS += -lpthreadVSE2
    LIBS += -L"D:/Windows Kits/8.0/Lib/win8/um/x86/" -lWS2_32
}
unix {
    LIBS += -lportaudio -lmp3lame -lshout
}

INCLUDEPATH += $$PWD/dep/portaudio/include
DEPENDPATH += $$PWD/dep/portaudio/include


win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/dep/lib/portaudio_x86.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/dep/lib/portaudio_x86.lib
else:unix: LIBS += -lportaudio -lmp3lame

win32::RC_FILE = w32app.rc
RESOURCES += \
    icons.qrc

unix|win32: LIBS += -L$$PWD/dep/lib/

INCLUDEPATH += $$PWD/dep/lame/include
DEPENDPATH += $$PWD/dep/lame/include

#win32: PRE_TARGETDEPS += $$PWD/dep/lib/delibmp3lame.lib
#else:unix: PRE_TARGETDEPS += 

INCLUDEPATH += $$PWD/dep/libshout/include
DEPENDPATH += $$PWD/dep/libshout/include

#win32: PRE_TARGETDEPS += $$PWD/dep/lib/libshout.lib
#else:unix: PRE_TARGETDEPS += 