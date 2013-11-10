import QtQml 2.0

QtObject {

    property string version: "0.7"
    property int versionCode: 7
    //最新版のダウンロードURL（複数あるときは「|」で区切る
    property variant downloadUrl: ["http://relog.xii.jp/download/kancolle/KanmusuMemory-0.7-win.zip"    // Win 32bit
                                , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.7-win.zip"     // Win 64bit
                                , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.7-mac.dmg"     // Mac 32bit
                                , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.7-mac.dmg"     // Mac 64bit
                                , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.7-ubuntu-x86.zip"
                                + "|http://relog.xii.jp/download/kancolle/kanmusumemory-0.7-ubuntu-x86.deb" //Linux 32bit
                                , "http://relog.xii.jp/download/kancolle/kanmusumemory-0.7-ubuntu-x86.deb"
                                + "|http://relog.xii.jp/download/kancolle/kanmusumemory-0.7-ubuntu-x86-64.deb" //Linux 64bit
                                ]
    property string webpageUrl: "http://relog.xii.jp/mt5r/2013/08/post-349.html"

}

