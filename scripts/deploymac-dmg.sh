#!/bin/bash

APPNAME=KanmusuMemory.app/
QTPATH=/Users/iori/Qt5.3.1/5.3/clang_64/

#clean
rm -rf KanmusuMemory.app
#rm KanmusuMemory-mac.zip
rm KanmusuMemory-mac.dmg

#build
${QTPATH}bin/qmake -r
make clean
make -r

mkdir KanmusuMemory.app/Contents/MacOS/i18n
cp i18n/qt_ja_JP.qm KanmusuMemory.app/Contents/MacOS/i18n/
cp i18n/qt_ja.qm KanmusuMemory.app/Contents/MacOS/i18n/

#deploy
${QTPATH}bin/macdeployqt KanmusuMemory.app -qmldir=./qml/KanmusuMemory

#archive
mkdir dmg
ln -s /Applications dmg/Applications
cp -rf KanmusuMemory.app Readme.txt dmg
hdiutil create -srcfolder ./dmg -fs HFS+ \
    -format UDZO -imagekey zlib-level=9 \
    -volname "Kanmusu Memory" KanmusuMemory-mac.dmg
rm -rf dmg

#zip KanmusuMemory-mac.zip KanmusuMemory.app Readme.txt
#zip KanmusuMemory-mac.zip resources/alarm.mp3 -j


