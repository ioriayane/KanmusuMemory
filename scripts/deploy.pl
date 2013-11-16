

use File::Find;
use Cwd;

#引数でOSを判定
$OS="";
for ($i = 0; $i <= $#ARGV; $i++) {
	if($ARGV[$i] eq "win32"){
		$OS = "win";
		$PTRSIZE = 32;
	}elsif($ARGV[$i] eq "win64"){
		$OS = "win";
		$PTRSIZE = 64;
	}elsif($ARGV[$i] eq "ubuntu32"){
		$OS = "ubuntu";
		$PTRSIZE = 32;
	}elsif($ARGV[$i] eq "ubuntu64"){
		$OS = "ubuntu";
		$PTRSIZE = 64;
	}elsif($ARGV[$i] eq "mac"){
		$OS = "mac";
		$PTRSIZE = 64;
	}else{
		print "invalid parameter\n";
		print "deploy win32|win64|ubuntu32|ubuntu64|mac'\n";
		
		exit;
	}
}

################################################
# OSごとの設定
################################################
if($OS eq "win"){
	################################################
	# Windowsの設定
	################################################

	# 実行ファイル名
	$EXENAME="KanmusuMemory.exe";
	# 実行ファイルができるディレクトリ
	$EXEDIR="release\\";
	# デプロイ先のディレクトリ
	$OUTDIR='..\\KanmusuMemoryBin\\KanmusuMemory\\';
	# 言語ファイルを保存しているディレクトリ
	$I18N="i18n\\";


	# Qtのディレクトリ
	$QTDIR="C:\\Qt\\Qt5.1.1vs12-32\\5.1.1\\msvc2012\\";
	# Qtのバイナリの場所
	$QTBIN="bin\\";
	# Qtのライブラリ（Winならdll, Ubuntuならso）の保存場所
	$QTLIB="bin\\";
	# QtQuickのプラグインの保存場所
	$QTQML="qml\\";
	# プラグインの保存場所
	$QTPLUGINS="plugins\\";

	# ライブラリの拡張子
	@LIBEXTS=(".dll");

	# 環境に依存したファイル
	if($PTRSIZE == 32){
		$PTRSIZE_NAME="x86";
	}else{
		$PTRSIZE_NAME="x64";
	}
	@PLATFORM_LIBS=("C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\redist\\" . $PTRSIZE_NAME . "\\Microsoft.VC110.CRT\\msvcp110.dll"
				  , "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\redist\\" . $PTRSIZE_NAME . "\\Microsoft.VC110.CRT\\msvcr110.dll"
				  , "..\\ssl_dll\\libssl32.dll"
				  , "..\\ssl_dll\\libeay32.dll"
				  , "..\\ssl_dll\\ssleay32.dll"
				  );
	# 環境ごとのコマンドの設定
	$MAKE="nmake";					# makeコマンド
	$CP="copy";						# 単品コピー
	$COPY="xcopy /S /E /I /Y";		# 複数コピー
	$MKDIR="mkdir";					# ディレクトリ作成
	$RMDIR="rmdir /S /Q";			# ディレクトリ削除
	$RM="del /F";					# ファイル削除

	# ファイルをコピーした最後に消すファイルのフィルタ
	@REMOVE_FILES=(
		"d.dll\$"
		, ".pdb\$"
		, "plugins.qmltypes"
		, "qoffscreen.dll"		#platforms
		, "qminimal.dll"		#platforms
		, "dsengine.dll"		#mediaservice
		);

}elsif($OS eq "ubuntu"){
	################################################
	# Ubuntuの設定
	################################################


	exit;
#}elsif($OS eq "mac"){
}else{
	print "invalid parameter\n";
	print "deploy win32|win64|ubuntu32|ubuntu64|mac'\n";

	exit;
}


#setup QML modules
@QML_MODULE=(
	  "Qt"
#	, "QtGraphicalEffects"
	, "QtMultimedia"
#	, "QtQml"
	, "QtQuick.2"
#	, "QtSensors"
#	, "QtTest"
#	, "QtWebKit"
	);


#sub folder "qml/QtQuick"
@QML_MODULE_QTQUICK=(
	  "Controls"
#	, "Dialogs"
	, "Layouts"
#	, "LocalStorage"
#	, "Particles.2"
#	, "PrivateWidgets"
#	, "Window.2"
#	, "XmlListModel"
	);

#setup Qt plugins
@QT_MODULE_PLUGIN=(
#	  "accessible"
#	, "bearer"
#	, "designer"
#	, "iconengines"
#	,
	  "imageformats"
	, "mediaservice"
	, "platforms"
#	, "playlistformats"
#	, "printsupport"
#	, "qml1tooling"
#	, "qmltooling"
#	, "sensorgestures"
#	, "sensors"
#	, "sqldrivers"
	);

#setup Qt libraries
@QT_MODULE_LIBRARY=(
	"d3dcompiler_46"
	, "icudt51", "icuin51", "icuuc51"
	, "libEGL", "libGLESv2"
	, "Qt0TwitterAPI"
#	, "Qt5CLucene"
#	, "Qt5Concurrent"
	, "Qt5Core"
#	, "Qt5Declarative"
#	, "Qt5Designer"
#	, "Qt5DesignerComponents"
	, "Qt5Gui"
#	, "Qt5Help"
	, "Qt5Multimedia", "Qt5MultimediaQuick_p", "Qt5MultimediaWidgets"
	, "Qt5Network"
	, "Qt5OpenGL"
	, "Qt5PrintSupport"
	, "Qt5Qml", "Qt5Quick"
#	, "Qt5QuickParticles"
#	, "Qt5QuickTest"
#	, "Qt5Script"
#	, "Qt5ScriptTools"
	, "Qt5Sensors"
#	, "Qt5SerialPort"
	, "Qt5Sql"
#	, "Qt5Svg"
#	, "Qt5Test"
	, "Qt5V8"
	, "Qt5WebKit", "Qt5WebKitWidgets"
	, "Qt5Widgets"
#	, "Qt5Xml"
#	, "Qt5XmlPatterns"
	);



# カレントディレクトリを取得する
open(IN, "cd |");
while (<IN>) {
    $PWD=$_;
}
close(IN);
$PWD =~ s/\n//g;


##################################
#build
##################################

# クリーン
system($RMDIR . " " . $OUTDIR);
# リビルド
system($PALTFORM_SETUP);
system($QTDIR . $QTBIN . "qmake -r");
system("$MAKE clean");
system("$MAKE");


##################################
#deploy
##################################
# 実行ファイル
system($MKDIR . " " . $OUTDIR);

# Readme.txtとか付属品のコピー
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
@libs=@QML_MODULE;
$lib_dir=$QTQML;
foreach $lib (@libs) {
	system("$COPY $QTDIR$lib_dir$lib $OUTDIR$lib");
}

#sub folder "qml/QtQuick"
@libs=@QML_MODULE_QTQUICK;
$lib_dir=$QTQML;
$sub_dir="QtQuick\\";
foreach $lib (@libs) {
	system("$COPY $QTDIR$lib_dir$sub_dir$lib $OUTDIR$sub_dir$lib");
}


##################################
#setup Qt plugins
##################################
@libs=@QT_MODULE_PLUGIN;
$lib_dir=$QTPLUGINS;
foreach $lib (@libs) {
	system("$COPY $QTDIR$lib_dir$lib $OUTDIR$lib");
}


##################################
#setup Qt libraries
##################################
@libs=@QT_MODULE_LIBRARY;
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



##################################
# remove 
##################################
find(\&find_callback_remove, $OUTDIR);


# 検索したファイルを削除
sub find_callback_remove {

	foreach $filter (@REMOVE_FILES) {
#		if($File::Find::name =~ /d.dll$/){
		if($File::Find::name =~ /$filter/){
			$temp = $File::Find::name;
			if($OS eq "win"){
				$temp =~ s/\//\\/g;
			}
			print "$RM $_ \n";
			system("$RM $_");
		}
	}
}
