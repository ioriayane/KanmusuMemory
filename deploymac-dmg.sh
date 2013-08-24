#!/bin/bash

APPNAME=KanmusuMemory.app/
QTPATH=/Applications/Qt/Qt5.1.0/5.1.0/clang_64/

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

#deploy
${QTPATH}bin/macdeployqt KanmusuMemory.app/


#setup QML modules
mkdir ${APPNAME}Contents/qml 
#p_names=("Qt" "QtAudioEngine" "QtGraphicalEffects" "QtMultimedia" "QtQuick.2" "QtTest" "QtWebKit")
p_names=("Qt" "QtAudioEngine" "QtMultimedia" "QtQuick.2")
for p_name in ${p_names[@]}
do
cp -R ${QTPATH}qml/${p_name} ${APPNAME}Contents/qml    
done

#sub folder
mkdir ${APPNAME}/Contents/qml/QtQuick 
#p_names=("LocalStorage" "Particles.2" "Window.2" "XmlListModel")
p_names=("Controls" "Layouts")
for p_name in ${p_names[@]}
do
cp -R ${QTPATH}qml/QtQuick/${p_name} ${APPNAME}Contents/qml/QtQuick
done

#setup Qt plugins
#p_names=("mediaservice" "platforms" "sqldrivers" "accessible" "bearer" "designer" "iconengines" "imageformats" "playlistformats" "printsupport" "qmltooling")
p_names=("mediaservice")
for p_name in ${p_names[@]}
do
cp -R ${QTPATH}plugins/${p_name} ${APPNAME}Contents/PlugIns
done


#setup Qt libraries
qtlibs=("QtMultimediaWidgets" "QtMultimedia" "QtMultimediaQuick_p")
for qtlib in ${qtlibs[@]}
do
cp -R ${QTPATH}lib/${qtlib}.framework ${APPNAME}Contents/Frameworks                   
done


#link recovery
cat scripts/deploymaclibs.txt | xargs -n1 scripts/macdeployhelper510.sh 

#find KanmusuMemory.app -name "*.dylib"
#find KanmusuMemory.app/Contents/Frameworks -name "Qt*" -type f

#remove unnecessary 
find ${APPNAME} -name "*.prl" | xargs rm 
find ${APPNAME} -name "*_debug*" | xargs rm 
find ${APPNAME} -name "*.qmltypes" | xargs rm 
find ${APPNAME} -name "Headers" | xargs rm -r 

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


