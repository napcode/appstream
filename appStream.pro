#-------------------------------------------------
#
# Project created by QtCreator 2012-11-22T20:11:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = appStream
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp \
    audiosystem.cpp \
    serverconnectiondialog.cpp \
    streaminfodialog.cpp \
    dsp.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h \
    audiosystem.h \
    serverconnectiondialog.h \
    streaminfodialog.h \
    ringbuffer.h \
    dsp.h

FORMS    += mainwindow.ui \
    settingsdialog.ui \
    serverconnectiondialog.ui \
    streaminfodialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/dep/lib/ -lportaudio_x86
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/dep/lib/ -lportaudio_x86
else:unix: LIBS += -lportaudio

INCLUDEPATH += $$PWD/dep/portaudio/include
DEPENDPATH += $$PWD/dep/portaudio/include

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/dep/lib/portaudio_x86.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/dep/lib/portaudio_x86.lib
else:unix: LIBS += -lportaudio

win32::RC_FILE = w32app.rc
RESOURCES += \
    icons.qrc
