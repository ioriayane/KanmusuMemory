#-------------------------------------------------
#
# Project created by QtCreator 2013-08-10T23:06:07
#
#-------------------------------------------------

QT       += core gui network webkitwidgets twitterapi

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KanmusuMemory
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    tweetdialog.cpp \
    inputoauthpindialog.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    tweetdialog.h \
    inputoauthpindialog.h \
    twitterinfo.h \
    settingsdialog.h

FORMS    += mainwindow.ui \
    tweetdialog.ui \
    inputoauthpindialog.ui \
    settingsdialog.ui

# icon
win32{
    RC_FILE = KanmusuMemory.rc
}
mac{
#    ICON = KanmusuMemory.icns
}
