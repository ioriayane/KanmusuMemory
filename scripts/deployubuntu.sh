#!/bin/bash

APPNAME=KanmusuMemory
TMPPATH=tmp
QTPATH=~/Qt5.1.0/5.1.0/gcc_64
LIBSETPATH=../KanmusuMemoryBin/KanmusuMemory

#clean
rm -rf ${APPNAME}
rm ${APPNAME}-0.0-ubuntu-x86.zip
#${QTPATH}/bin/qmake -r
#make clean
make

#deploy
mkdir ${TMPPATH}

#copy libs
cp -R ${LIBSETPATH}/* ${TMPPATH}/

#copy tmp files
cp Readme.txt ${TMPPATH}/
cp resources/alarm.mp3 ${TMPPATH}/bin
cp scripts/KanmusuMemory.sh ${TMPPATH}/bin
cp i18n/qt_ja_JP.qm ${TMPPATH}/bin/i18n/

#copy bin
mv ${APPNAME} ${TMPPATH}/bin/

#rename dir
mv ${TMPPATH} ${APPNAME}

#archive
zip -ry ${APPNAME}-0.0-ubuntu-x86.zip ${APPNAME}




