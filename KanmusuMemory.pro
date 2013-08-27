TEMPLATE = app
TARGET = KanmusuMemory
QT       += core gui network webkitwidgets twitterapi
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    tweetdialog.cpp \
    inputoauthpindialog.cpp \
    settingsdialog.cpp \
    cookiejar.cpp \
    aboutdialog.cpp \
    memorydialog.cpp \
    timerdialog.cpp

HEADERS  += mainwindow.h \
    tweetdialog.h \
    inputoauthpindialog.h \
    ../KanmusuMemoryTwitter/twitterinfo.h \
    settingsdialog.h \
    cookiejar.h \
    aboutdialog.h \
    memorydialog.h \
    timerdialog.h \
    kanmusumemory_global.h

FORMS    += mainwindow.ui \
    tweetdialog.ui \
    inputoauthpindialog.ui \
    settingsdialog.ui \
    aboutdialog.ui \
    memorydialog.ui \
    timerdialog.ui


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
