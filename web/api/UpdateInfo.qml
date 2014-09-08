/*
 * Copyright 2013-2014 KanMemo Project.
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

    property string version: "0.1.0"
    property int versionCode: 1
    //最新版のダウンロードURL（複数あるときは「|」で区切る
    property variant downloadUrl: [
        "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.15.1-win-x86.zip"    // Win 32bit
        , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.15.1-win-x86-64.zip"     // Win 64bit
        , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.15.1-mac.dmg"     // Mac 32bit
        , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.15.1-mac.dmg"     // Mac 64bit
        , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.15.1-ubuntu-x86.zip"
        + "|http://relog.xii.jp/download/kancolle/kanmusumemory-0.15.1-ubuntu-x86.deb" //Linux 32bit
        , "http://relog.xii.jp/download/kancolle/KanmusuMemory-0.15.1-ubuntu-x86-64.zip"
        + "|http://relog.xii.jp/download/kancolle/kanmusumemory-0.15.1-ubuntu-x86-64.deb" //Linux 64bit
    ]
    property string webpageUrl: "http://relog.xii.jp/"
    property variant message: ["- new."
        , "・new"             //日本語の説明
    ]
    property string lastFavoriteData: "2014090800"
    property string lastTimerSelectGuide: "2013031400"
    property string lastRecognizeInfo: "2013081100"
}

