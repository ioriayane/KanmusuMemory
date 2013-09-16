TEMPLATE = app
TARGET = KanmusuMemory
QT       += core gui network webkitwidgets twitterapi
CONFIG += c++11

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
    imageeditdialog.cpp

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
    imageeditdialog.h

FORMS    += mainwindow.ui \
    tweetdialog.ui \
    settingsdialog.ui \
    aboutdialog.ui \
    memorydialog.ui \
    timerdialog.ui \
    imageeditdialog.ui


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
