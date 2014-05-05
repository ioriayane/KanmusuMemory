/*
 * Copyright 2013 KanMemo Project.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import QtQml 2.0

QtObject {

    property string version: "0.12"
    property int versionCode: 13
    //最新版のダウンロードURL（複数あるときは「|」で区切る
    property variant downloadUrl: [
        "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.12-win-x86.zip"    // Win 32bit
        , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.12-win-x86-64.zip"     // Win 64bit
        , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.12-mac.dmg"     // Mac 32bit
        , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.12-mac.dmg"     // Mac 64bit
        , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.12-ubuntu-x86.zip"
        + "|http://relog.xii.jp/download/kancolle/kanmusumemory-0.12-ubuntu-x86.deb" //Linux 32bit
        , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.12-ubuntu-x86-64.zip"
        + "|http://relog.xii.jp/download/kancolle/kanmusumemory-0.12-ubuntu-x86-64.deb" //Linux 64bit
    ]
    property string webpageUrl: "http://relog.xii.jp/mt5r/2013/08/post-349.html"
    property variant message: ["-Adding a function to display the screen veterans report\n-Bug fix\n"
        , "・戦果報告画面のコピーを進撃・撤退選択画面まで表示する機能の追加\n・キャッシュの強制削除機能の追加\n・分割画面にタブが1つもない状態でCtrl+Rを押すと強制終了する不具合の修正\n・アップデート確認機能の内部処理を修正\n・開発環境をQt 5.2.1に変更\n"             //日本語の説明
    ]
    property string lastFavoriteData: "2014042600"
    property string lastTimerSelectGuide: "2014031400"
}


