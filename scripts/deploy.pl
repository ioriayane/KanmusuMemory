


$EXENAME="KanmusuMemory.exe";
$EXEDIR="release\\";
$OUTDIR='..\\KanmusuMemoryBin\\Kanmemo\\';
$QTDIR="C:\\Qt\\Qt5.1.1vs12-32\\5.1.1\\msvc2012\\";
$I18N="i18n\\";
$QTLIB="bin\\";
$QTQML="qml\\";
$QTPLUGINS="plugins\\";

@LIBEXTS=(".dll");

$PLATFORM_SETUP="\"c:\Program Files (x86)\Microsoft Visual Studio 11.0\vc\vcvarsall.bat\" x86";
$ARCSIZE="x86";	#x64
@PLATFORM_LIBS=("C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\redist\\" . $ARCSIZE . "\\Microsoft.VC110.CRT\\msvcp110.dll"
			  , "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\redist\\" . $ARCSIZE . "\\Microsoft.VC110.CRT\\msvcr110.dll"
			  , "..\\ssl_dll\\libssl32.dll"
			  , "..\\ssl_dll\\libeay32.dll"
			  , "..\\ssl_dll\\ssleay32.dll"
			  );
$MAKE="nmake";
$CP="copy";
$COPY="xcopy /S /E /I /Y";
$MKDIR="mkdir";
$RMDIR="rmdir /S /Q";


open(IN, "cd |");
while (<IN>) {
    $PWD=$_;
}
close(IN);
$PWD =~ s/\n//g;

#clean
system($RMDIR . " " . $OUTDIR);

#build
system($PALTFORM_SETUP);
system("qmake -r");
system("$MAKE clean");
system("$MAKE");

##################################
#deploy
##################################
system($MKDIR . " " . $OUTDIR);

system("$CP $EXEDIR$EXENAME $OUTDIR");
system("$CP $PWD" . "\\Readme.txt $OUTDIR");
system("$CP resources\\alarm.mp3 $OUTDIR");


##################################
#setup lang
##################################
system("$MKDIR $OUTDIR$I18N");
system("$COPY $I18N" . "*.qm $OUTDIR$I18N");


##################################
#setup QML modules
##################################
#@libs=("Qt", "QtGraphicalEffects", "QtMultimedia", "QtQml", "QtQuick.2", "QtSensors", "QtTest", "QtWebKit");
@libs=("Qt", "QtMultimedia", "QtQuick.2");
$lib_dir="qml\\";
foreach $lib (@libs) {
	system("$COPY $QTDIR$lib_dir$lib $OUTDIR$lib");
}

#sub folder "qml/QtQuick"
#@libs=("Controls", "Dialogs", "Layouts", "LocalStorage", "Particles.2", "PrivateWidgets", "Window.2" "XmlListModel")
@libs=("Controls", "Layouts");
$lib_dir=$QTQML;
$sub_dir="QtQuick\\";
foreach $lib (@libs) {
	system("$COPY $QTDIR$lib_dir$sub_dir$lib $OUTDIR$sub_dir$lib");
}


##################################
#setup Qt plugins
##################################
#@libs=("accessible", "bearer", "designer", "iconengines", "imageformats", "mediaservice", "platforms", "playlistformats", "printsupport", "qml1tooling", "qmltooling", "sensorgestures", "sensors", "sqldrivers");
@libs=("imageformats", "mediaservice", "platforms");
$lib_dir=$QTPLUGINS;
foreach $lib (@libs) {
	system("$COPY $QTDIR$lib_dir$lib $OUTDIR$lib");
}


##################################
#setup Qt libraries
##################################
#@libs=("d3dcompiler_46", "icudt51", "icuin51", "icuuc51", "libEGL", "libGLESv2", "Qt0TwitterAPI", "Qt5CLucene", "Qt5Concurrent", "Qt5Core", "Qt5Declarative", "Qt5Designer", "Qt5DesignerComponents", "Qt5Gui", "Qt5Help", "Qt5Multimedia", "Qt5MultimediaQuick_p", "Qt5MultimediaWidgets", "Qt5Network", "Qt5OpenGL", "Qt5PrintSupport", "Qt5Qml", "Qt5Quick", "Qt5QuickParticles", "Qt5QuickTest", "Qt5Script", "Qt5ScriptTools", "Qt5Sensors", "Qt5SerialPort", "Qt5Sql", "Qt5Svg", "Qt5Test", "Qt5V8", "Qt5WebKit", "Qt5WebKitWidgets", "Qt5Widgets", "Qt5Xml", "Qt5XmlPatterns");

@libs=("d3dcompiler_46", "icudt51", "icuin51", "icuuc51", "libEGL", "libGLESv2", "Qt0TwitterAPI", "Qt5Core", "Qt5Gui", "Qt5Multimedia", "Qt5MultimediaQuick_p", "Qt5MultimediaWidgets", "Qt5Network", "Qt5OpenGL", "Qt5PrintSupport", "Qt5Qml", "Qt5Quick", "Qt5Sensors", "Qt5Sql", "Qt5V8", "Qt5WebKit", "Qt5WebKitWidgets", "Qt5Widgets");
$lib_dir=$QTLIB;
foreach $lib (@libs) {
	foreach $libext (@LIBEXTS) {
		system("$COPY $QTDIR$lib_dir$lib$libext $OUTDIR");
	}
}


##################################
#setup platform deplend files
##################################
foreach $plat_lib (@PLATFORM_LIBS) {
	system("$CP \"$plat_lib\" $OUTDIR");
}

