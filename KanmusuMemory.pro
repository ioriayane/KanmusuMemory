TEMPLATE = app
TARGET = KanmusuMemory
QT       += core gui network webkitwidgets twitterapi
CONFIG += c++11

INCLUDEPATH += ../KanmusuMemoryTwitter

SOURCES += main.cpp\
        mainwindow.cpp \
    tweetdialog.cpp \
    inputoauthpindialog.cpp \
    settingsdialog.cpp \
    cookiejar.cpp \
    aboutdialog.cpp \
    memorydialog.cpp

HEADERS  += mainwindow.h \
    tweetdialog.h \
    inputoauthpindialog.h \
    twitterinfo.h \
    settingsdialog.h \
    cookiejar.h \
    aboutdialog.h \
    memorydialog.h

FORMS    += mainwindow.ui \
    tweetdialog.ui \
    inputoauthpindialog.ui \
    settingsdialog.ui \
    aboutdialog.ui \
    memorydialog.ui


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
