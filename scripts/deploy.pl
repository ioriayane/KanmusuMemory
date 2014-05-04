

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
#	, "Qt5CLucene"
#	, "Qt5Concurrent"
	 "Qt5Core"
    , "Qt5DBus"
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


#32bit or 64bit
if($PTRSIZE == 32){
	$PTRSIZE_NAME="x86";
}else{
	$PTRSIZE_NAME="x64";
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
	$OUTDIR='..\\KanmusuMemoryBin\\KanmusuMemory' . $PTRSIZE_NAME . '\\';
    $OUTDIRBIN="";
    $OUTDIRLIB="";
	# 言語ファイルを保存しているディレクトリ
	$I18N="i18n\\";


	# Qtのディレクトリ
	if($PTRSIZE_NAME eq "x86"){
		$QTDIR="C:\\Qt\\Qt5.2.1vs12-32\\5.2.1\\msvc2012\\";
	}else{
		$QTDIR="C:\\Qt\\Qt5.2.1vs12-64\\5.2.1\\msvc2012_64\\";
	}
	# Qtのバイナリの場所
	$QTBIN="bin\\";
	# Qtのライブラリ（Winならdll, Ubuntuならso）の保存場所
	$QTLIB="bin\\";
	# QtQuickのプラグインの保存場所
	$QTQML="qml\\";
	# プラグインの保存場所
	$QTPLUGINS="plugins\\";

	# 環境に依存したファイル
	@PLATFORM_LIBS=("C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\redist\\" . $PTRSIZE_NAME . "\\Microsoft.VC110.CRT\\msvcp110.dll"
				  , "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\redist\\" . $PTRSIZE_NAME . "\\Microsoft.VC110.CRT\\msvcr110.dll"
#				  , "..\\ssl_dll\\". $PTRSIZE_NAME . "\\libssl32.dll"
				  , "..\\ssl_dll\\". $PTRSIZE_NAME . "\\libeay32.dll"
				  , "..\\ssl_dll\\". $PTRSIZE_NAME . "\\ssleay32.dll"
                  , "resources\\alarm.mp3"
				  );

	# 環境ごとのコマンドの設定
	if($PTRSIZE_NAME eq "x86"){
		$MAKE="c:\\qt\\Qt5.2.1vs12-32\\Tools\\QtCreator\\bin\\jom.exe";			# makeコマンド
	}else{
		$MAKE="c:\\qt\\Qt5.2.1vs12-64\\Tools\\QtCreator\\bin\\jom.exe";			# makeコマンド
	}
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

    # 名称の修正
    @temp = ();
    foreach $name (@QT_MODULE_LIBRARY){
        push(@temp, $name . ".dll");
    }
    @QT_MODULE_LIBRARY = @temp;

    # 追加（不足分）
    push(@QT_MODULE_LIBRARY, "Qt0TwitterAPI.dll");
    push(@QT_MODULE_LIBRARY, "Qt5MultimediaQuick_p.dll");

}elsif($OS eq "ubuntu"){
	################################################
	# Ubuntuの設定
	################################################

	# 実行ファイル名
	$EXENAME="KanmusuMemory";
	# 実行ファイルができるディレクトリ
	$EXEDIR="";
	# デプロイ先のディレクトリ
	$OUTDIR='tmp/';
    $OUTDIRBIN="bin/";
    $OUTDIRLIB="lib/";
	# 言語ファイルを保存しているディレクトリ
	$I18N="i18n/";


	# Qtのディレクトリ
	if($PTRSIZE == 32){
		$QTDIR="~/Qt5.2.1/5.2.1/gcc/";
	}else{
		$QTDIR="~/Qt5.2.1/5.2.1/gcc_64/";
	}
	# Qtのバイナリの場所
	$QTBIN="bin/";
	# Qtのライブラリ（Winならdll, Ubuntuならso）の保存場所
	$QTLIB="lib/";
	# QtQuickのプラグインの保存場所
	$QTQML="qml/";
	# プラグインの保存場所
	$QTPLUGINS="plugins/";

	# 環境に依存したファイル
	@PLATFORM_LIBS=(
                    "resources/alarm.mp3"
                    , "scripts/KanmusuMemory.sh"
				  );

	# 環境ごとのコマンドの設定
	$MAKE="make";				# makeコマンド
	$CP="cp";					# 単品コピー
	$COPY="cp -rf";	            # 複数コピー
	$MKDIR="mkdir";				# ディレクトリ作成
	$RMDIR="rm -rf";			# ディレクトリ削除
	$RM="rm";					# ファイル削除

	# ファイルをコピーした最後に消すファイルのフィルタ
	@REMOVE_FILES=(
		"plugins.qmltypes"
        , "libqlinuxfb.so"        #platforms
        , "libqminimal.so"      #platforms
        , "libqoffscreen.so"    #platforms
		);


    # 名称の修正
    @temp = ();
    foreach $name (@QT_MODULE_LIBRARY){
        push(@temp, "lib" . $name . ".so.5");
        push(@temp, "lib" . $name . ".so.5.1.0");
    }
    @QT_MODULE_LIBRARY = @temp;
    
    # 追加ライブラリ
    push(@QT_MODULE_PLUGIN, "platforminputcontexts");
    push(@QT_MODULE_PLUGIN, "platformthemes");

    push(@QT_MODULE_LIBRARY, "libQt0TwitterAPI.so.0");
    push(@QT_MODULE_LIBRARY, "libQt0TwitterAPI.so.0.1.0");
    push(@QT_MODULE_LIBRARY, "libQt5MultimediaQuick_p.so.0");
    push(@QT_MODULE_LIBRARY, "libQt5MultimediaQuick_p.so.0.1.0");
#    push(@QT_MODULE_LIBRARY, "libicudata.so.51");
#    push(@QT_MODULE_LIBRARY, "libicudata.so.51.1");
#    push(@QT_MODULE_LIBRARY, "libicui18n.so.51");
#    push(@QT_MODULE_LIBRARY, "libicui18n.so.51.1");
#    push(@QT_MODULE_LIBRARY, "libicuuc.so.51");
#    push(@QT_MODULE_LIBRARY, "libicuuc.so.51.1");
#    push(@QT_MODULE_LIBRARY, "libqgsttools_p.so.1");
#    push(@QT_MODULE_LIBRARY, "libqgsttools_p.so.1.0.0");

#}elsif($OS eq "mac"){
}else{
	print "invalid parameter\n";
	print "deploy win32|win64|ubuntu32|ubuntu64|mac'\n";

	exit;
}





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
system($QTDIR . $QTBIN . "qmake -r");
system("$MAKE clean");
system("$MAKE");


##################################
#deploy
##################################
# 出力先作成
system("$MKDIR $OUTDIR");
if($OS eq "ubuntu"){
    system("$MKDIR $OUTDIR$OUTDIRBIN");
    system("$MKDIR $OUTDIR$OUTDIRLIB");
}

# 実行ファイル
system("$CP $EXEDIR$EXENAME $OUTDIR$OUTDIRBIN");

# Readme.txtとか付属品のコピー
if($OS eq "win"){
    system("$CP $PWD" . "\\Readme.txt $OUTDIR");
#    system("$CP resources\\alarm.mp3 $OUTDIR");
}elsif($OS eq "ubuntu"){
    system("$CP $PWD" . "/Readme.txt $OUTDIR");
}else{
}

##################################
#setup lang
##################################
system("$MKDIR $OUTDIR$OUTDIRBIN$I18N");
system("$COPY $I18N" . "*.qm $OUTDIR$OUTDIRBIN$I18N");

if($OS eq "win"){

	system("windeployqt --qmldir qml --dir $OUTDIR $EXEDIR$EXENAME");

}else{


	##################################
	#setup QML modules
	##################################
	@libs=@QML_MODULE;
	$lib_dir=$QTQML;
	foreach $lib (@libs) {
		system("$COPY $QTDIR$lib_dir$lib $OUTDIR$OUTDIRBIN$lib");
	}

	#sub folder "qml/QtQuick"
	@libs=@QML_MODULE_QTQUICK;
	$lib_dir=$QTQML;
	if($OS eq "win"){
	    $sub_dir="QtQuick\\";
	}else{
	    $sub_dir="QtQuick/";
	}
	if($#libs >= 0){
	    system("$MKDIR $OUTDIR$OUTDIRBIN$sub_dir");
	    foreach $lib (@libs) {
		    system("$COPY $QTDIR$lib_dir$sub_dir$lib $OUTDIR$OUTDIRBIN$sub_dir$lib");
	    }
	}

	##################################
	#setup Qt plugins
	##################################
	@libs=@QT_MODULE_PLUGIN;
	$lib_dir=$QTPLUGINS;
	foreach $lib (@libs) {
		system("$COPY $QTDIR$lib_dir$lib $OUTDIR$OUTDIRBIN$lib");
	}
}

##################################
#setup Qt libraries
##################################
@libs=@QT_MODULE_LIBRARY;
$lib_dir=$QTLIB;
foreach $lib (@libs) {
	system("$COPY $QTDIR$lib_dir$lib$libext $OUTDIR$OUTDIRLIB");
}

##################################
#setup platform depend files
##################################
foreach $plat_lib (@PLATFORM_LIBS) {
	system("$CP \"$plat_lib\" $OUTDIR$OUTDIRBIN");
}


##################################
# remove 
##################################
find(\&find_callback_remove, $OUTDIR);


# 検索したファイルを削除
sub find_callback_remove {

	foreach $filter (@REMOVE_FILES) {
		if($File::Find::name =~ /$filter/){
			$temp = $File::Find::name;
			if($OS eq "win"){
				$temp =~ s/\//\\/g;
			}
			#print "$RM $_ \n";
			system("$RM $_");
		}
	}
}
