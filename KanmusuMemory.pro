TEMPLATE = app
TARGET = KanmusuMemory
QT       += core gui network webkitwidgets twitterapi multimedia
CONFIG += c++11

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp\
        mainwindow.cpp \
    tweetdialog.cpp \
    settingsdialog.cpp \
    cookiejar.cpp \
    aboutdialog.cpp \
    memorydialog.cpp \
    timerdialog.cpp \
    webview.cpp \
    gamescreen.cpp \
    imageeditdialog.cpp \
    webpageform.cpp \
    tabwidget.cpp \
    favoritemenu.cpp \
    updateinfodialog.cpp \
    webpage.cpp \
    fleetdetaildialog.cpp \
    recordingthread.cpp \
    recognizeinfo.cpp \
    audiorecorder.cpp \
    recordingdialog.cpp

HEADERS  += mainwindow.h \
    tweetdialog.h \
    ../KanmusuMemoryTwitter/twitterinfo.h \
    settingsdialog.h \
    cookiejar.h \
    aboutdialog.h \
    memorydialog.h \
    timerdialog.h \
    kanmusumemory_global.h \
    webview.h \
    gamescreen.h \
    imageeditdialog.h \
    webpageform.h \
    tabwidget.h \
    favoritemenu.h \
    updateinfodialog.h \
    webpage.h \
    fleetdetaildialog.h \
    recordingthread.h \
    numberguide.h \
    recognizeinfo.h \
    audiorecorder.h \
    recordingdialog.h

FORMS    += mainwindow.ui \
    tweetdialog.ui \
    settingsdialog.ui \
    aboutdialog.ui \
    memorydialog.ui \
    timerdialog.ui \
    imageeditdialog.ui \
    webpageform.ui \
    updateinfodialog.ui \
    fleetdetaildialog.ui \
    recordingdialog.ui


include(qtquick/qtquick.pri)
#qtcAddDeployment()


# icon
win32{
    RC_FILE = KanmusuMemory.rc
}
mac{
    ICON = KanmusuMemory.icns
}

RESOURCES += \
    KanmusuMemory.qrc

