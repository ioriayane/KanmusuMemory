#!/bin/bash

#update library link
Update(){
TARGET=`basename $1`
QTPATH=/Users/iori/Qt5.1.1//5.1.1/clang_64/
LIBPATH=$1
CMD1="install_name_tool -id"
CMD2="install_name_tool -change"
ABSPATH1=lib/
ABSPATH2=.framework/Versions/5/
RELPATH1=@executable_path/../Frameworks/
RELPATH2=.framework/Versions/5/

count=1
for arg in $@
do
if [ $count -gt 1 ] ; then
if [ $arg = $TARGET ] ; then
# -id
$CMD1 ${RELPATH1}${arg}${RELPATH2}${arg} $LIBPATH
else
# -change
$CMD2 ${QTPATH}${ABSPATH1}${arg}${ABSPATH2}${arg} ${RELPATH1}${arg}${RELPATH2}${arg} $LIBPATH 
fi
fi
count=$(($count+1))
# count=`expr $count + 1`
done
}

#pickup library name
rel=`otool -L $1 | grep -E '\t/.+Qt' | sed -e "s/(.*)//g"`
OIFS="$IFS"; IFS=' '
name_array=($rel); IFS="$OIFS"

name=""
for arg in ${name_array[@]}
do
name="$name `basename $arg`"
done

#output
echo "`basename $1` :$name"

#update
Update $1 $name

#check
otool -L $1 | grep Qt



# install_name_tool -change /Users/iori/Qt5.0.2/5.0.2/clang_64/lib/QtQuick.framework/Versions/5/QtQuick @executable_path/../Frameworks/QtQuick.framework/Versions/5/QtQuick HelloWorld.app/Contents/qml/QtQuick.2/libqtquick2plugin.dylib 
# install_name_tool -change /Users/iori/Qt5.0.2/5.0.2/clang_64/lib/QtQml.framework/Versions/5/QtQml @executable_path/../Frameworks/QtQml.framework/Versions/5/QtQml HelloWorld.app/Contents/qml/QtQuick.2/libqtquick2plugin.dylib 
# install_name_tool -change /Users/iori/Qt5.0.2/5.0.2/clang_64/lib/QtNetwork.framework/Versions/5/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/5/QtNetwork HelloWorld.app/Contents/qml/QtQuick.2/libqtquick2plugin.dylib 
#install_name_tool -change /Users/iori/Qt5.0.2/5.0.2/clang_64/lib/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore HelloWorld.app/Contents/qml/QtQuick.2/libqtquick2plugin.dylib 
# install_name_tool -change /Users/iori/Qt5.0.2/5.0.2/clang_64/lib/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui HelloWorld.app/Contents/qml/QtQuick.2/libqtquick2plugin.dylib 

