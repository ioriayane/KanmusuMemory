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
import QtQuick 2.0
import QtQuick.Controls 1.0

Rectangle {
    id: root
    width: timerArea.width * 1.1
    height: timerArea.height * 1.1
    color: "#f0f0f0"

    property real d0set: 0
    property real d0start: 0
    property real d1set: 7830000
    property real d1start: 0
    property real d2set: 0
    property real d2start: 0
    property real d3set: 0
    property real d3start: 0


    Timer {
        id: timer
        running: true
        interval: 1000
        repeat: true
        triggeredOnStart: true
        onTriggered: updateAll()
    }

    function updateAll(){
        var i=0;
        for(i=0; i<docking.count; i++){
            docking.itemAt(i).update()
        }
        for(i=0; i<expedition.count; i++){
            expedition.itemAt(i).update()
        }
        for(i=0; i<construction.count; i++){
            construction.itemAt(i).update()
        }
    }

    //qmlsceneで使用するテスト用データ
    //    QtObject {
    //        id: timerData
    //        //入渠
    //        property variant dockingTime: [0, 7830000, 0, 0]
    //        property variant dockingStart: [0, 0, 0, 0]
    //        property variant dockingRunning: [false, false, false, false]
    //        //遠征
    //        property variant expeditionTime: [0, 7830000, 0, 0]
    //        property variant expeditionStart: [0, 0, 0, 0]
    //        property variant expeditionRunning: [false, false, false, false]
    //        //建造
    //        property variant constructionTime: [0, 30000, 0, 0]
    //        property variant constructionStart: [0, 0, 0, 0]
    //        property variant constructionRunning: [false, true, false, false]

    //        function setTime(kind, index, time){
    //            switch(kind){
    //            case 0:
    //                dockingTime[index] = time
    //                break
    //            default:
    //                break
    //            }
    //        }
    //    }

    Column {
        id: timerArea
        anchors.centerIn: parent

        Item { width: 5; height: 5 }
        //入渠
        Text {
            text: qsTr("Docking")
            font.pointSize: 18
        }
        GroupBox {
            Column {
                Repeater {
                    id: docking
                    model: timerData.dockingTime.length
                    delegate: TimerItem {
                        setTime: timerData.dockingTime[index]              //指定時間
                        startTime: timerData.dockingStart[index]          //開始時間
                        onStarted: {
                            //                            console.debug("QML:start=" + startTime)
                            timerData.setStartTime(0, index, startTime)
                            timerData.setRunning(0, index, true)
                        }
                        onStopped: timerData.setRunning(0, index, false)
                        onSetting: root.openSetting(0, index, setTime)
                        onFinished: {
                            //                            console.debug("FINISH 0 " + index)
                        }
                        Component.onCompleted: {
                            //開始時に動いてるか
                            if(timerData.dockingRunning[index])
                                restart()
                        }
                    }
                }
            }
        }
        Item { width: 5; height: 5 }
        //遠征
        Text {
            text: qsTr("Expedition")
            font.pointSize: 18
        }
        GroupBox {
            Column {
                Repeater {
                    id: expedition
                    model: timerData.expeditionTime.length
                    delegate: TimerItem {
                        setTime: timerData.expeditionTime[index]          //指定時間
                        startTime: timerData.expeditionStart[index]      //開始時間
                        indexOffset: 1

                        onStarted: {
                            timerData.setStartTime(1, index, startTime)
                            timerData.setRunning(1, index, true)
                        }
                        onStopped: timerData.setRunning(1, index, false)
                        onSetting: root.openSetting(1, index, setTime)
                        onFinished: {
                            //                            console.debug("FINISH 1 " + index)
                        }
                        Component.onCompleted: {
                            //開始時に動いてるか
                            if(timerData.expeditionRunning[index])
                                restart()
                        }
                    }
                }
            }
        }
        Item { width: 5; height: 5 }
        //建造
        Text {
            text: qsTr("Construction")
            font.pointSize: 18
        }
        GroupBox {
            Column {
                Repeater {
                    id: construction
                    model: timerData.constructionTime.length
                    delegate: TimerItem {
                        setTime: timerData.constructionTime[index]          //指定時間
                        startTime: timerData.constructionStart[index]      //開始時間
                        onStarted: {
                            timerData.setStartTime(2, index, startTime)
                            timerData.setRunning(2, index, true)
                        }
                        onStopped: timerData.setRunning(2, index, false)
                        onSetting: root.openSetting(2, index, setTime)
                        onFinished: {
                            //                            console.debug("FINISH 2 " + index)
                        }
                        Component.onCompleted: {
                            //開始時に動いてるか
                            if(timerData.constructionRunning[index])
                                restart()
                        }
                    }
                }
            }
        }
        Item { width: 5; height: 5 }
        CheckBox {
            anchors.left: parent.left
            anchors.leftMargin: 10
            text: "tweet a time-out"
            checked: timerData.tweetFinished    //このバインドは初期値をもらうだけ
            onCheckedChanged: timerData.tweetFinished = checked
        }
    }
    //ダイアログ開く
    function openSetting(kind, index, settime){
        var d = new Date(settime)
        setting.forceActiveFocus()
        settingOpenClose.state = "open"
        //設定している対象を保存
        setting.kind = kind
        setting.index = index
        //スピンボックスに現在の値をセット
        setting.spinHour = d.getUTCHours()
        setting.spinMinute = d.getUTCMinutes()
        setting.spinSecond = d.getUTCSeconds()
        //モデルを変更
        switch(kind){
        case 0: //入渠
            setting.model = dockingModel
            break;
        case 1: //遠征
            setting.model = expeditionModel
            break;
        case 2: //建造
            setting.model = constructionModel
            break;
        default:
            break;
        }
    }
    //ダイアログ閉じる
    function closeSetting(){
        settingOpenClose.state = ""
        //        setting.x = root.width + 1
    }
    //設定を反映する
    function applySetTime(kind, index, hour, minute, second){
        var settime = Date.UTC(1970, 0, 1, hour, minute, second)
        switch(kind){
        case 0: //入渠
            docking.itemAt(index).stop()
            timerData.setTime(kind, index, settime)
            break;
        case 1: //遠征
            expedition.itemAt(index).stop()
            timerData.setTime(kind, index, settime)
            break;
        case 2: //建造
            construction.itemAt(index).stop()
            timerData.setTime(kind, index, settime)
            break;
        default:
            break;
        }
    }

    StateGroup {
        id: settingOpenClose
        states: [
            State {
                name: "open"
                PropertyChanges {
                    target: setting
                    x: 0
                    opacity: 1
                }
            }
        ]
    }

    //設定ダイアログ
    TimerSetting {
        id: setting
        x: root.width + 1
        width: root.width
        height: root.height
        //        visible: false
        model: constructionModel

        onAccepted: root.applySetTime(kind, index, hour, minute, second)
        onClose: root.closeSetting()
    }

    ListModel {
        id: dockingModel
        ListElement { hour: 0; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 0; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 0; minute: 45; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 1; minute: 0; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 1; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 1; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 1; minute: 45; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 2; minute: 0; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 2; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 2; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 2; minute: 45; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 3; minute: 0; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 3; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 3; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 3; minute: 45; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 4; minute: 0; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 4; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 4; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 4; minute: 45; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 5; minute: 0; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 5; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 5; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 5; minute: 45; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 6; minute: 0; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 6; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 6; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 6; minute: 45; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 7; minute: 0; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 7; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 7; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 7; minute: 45; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 8; minute: 0; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 8; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 8; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 8; minute: 45; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 9; minute: 0; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 9; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 9; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 9; minute: 45; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 10; minute: 0; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 10; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 10; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 10; minute: 45; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 11; minute: 0; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 11; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 11; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 11; minute: 45; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 12; minute: 0; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 12; minute: 15; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 12; minute: 30; second: 0; msg1: ""; msg2: "" }
        ListElement { hour: 12; minute: 45; second: 0; msg1: ""; msg2: "" }
    }
    //遠征
    ListModel {
        id: expeditionModel
        ListElement { hour: 0; minute: 15; second: 0; msg1: "鎮守府海域"; msg2: "1 練習航海" }
        ListElement { hour: 0; minute: 30; second: 0; msg1: "鎮守府海域"; msg2: "2 長距離練習航海" }
        ListElement { hour: 0; minute: 20; second: 0; msg1: "鎮守府海域"; msg2: "3 警備任務" }
        ListElement { hour: 0; minute: 50; second: 0; msg1: "鎮守府海域"; msg2: "4 対潜警戒任務" }
        ListElement { hour: 1; minute: 30; second: 0; msg1: "鎮守府海域"; msg2: "5 海上護衛任務" }
        ListElement { hour: 0; minute: 40; second: 0; msg1: "鎮守府海域"; msg2: "6 防空射撃演習" }
        ListElement { hour: 1; minute: 0; second: 0; msg1: "鎮守府海域"; msg2: "7 観艦式予行" }
        ListElement { hour: 3; minute: 0; second: 0; msg1: "鎮守府海域"; msg2: "8 観艦式" }

        ListElement { hour: 4; minute: 0; second: 0; msg1: "南西諸島海域"; msg2: "9 タンカー護衛任務" }
        ListElement { hour: 1; minute: 30; second: 0; msg1: "南西諸島海域"; msg2: "10 強行偵察任務" }
        ListElement { hour: 5; minute: 0; second: 0; msg1: "南西諸島海域"; msg2: "11 ボーキサイト輸送任務" }
        ListElement { hour: 8; minute: 0; second: 0; msg1: "南西諸島海域"; msg2: "12 資源輸送任務" }
        ListElement { hour: 4; minute: 0; second: 0; msg1: "南西諸島海域"; msg2: "13 鼠輸送作成" }
        ListElement { hour: 6; minute: 0; second: 0; msg1: "南西諸島海域"; msg2: "14 包囲陸戦隊撤収作戦" }
        ListElement { hour: 12; minute: 0; second: 0; msg1: "南西諸島海域"; msg2: "15 囮機動部隊支援作戦" }
        ListElement { hour: 15; minute: 0; second: 0; msg1: "南西諸島海域"; msg2: "16 艦隊決戦援護作戦" }

        ListElement { hour: 0; minute: 45; second: 0; msg1: "北方海域"; msg2: "17 敵地偵察作戦" }
        ListElement { hour: 5; minute: 0; second: 0; msg1: "北方海域"; msg2: "18 航空機輸送作戦" }
        ListElement { hour: 6; minute: 0; second: 0; msg1: "北方海域"; msg2: "19 北号作戦" }
        ListElement { hour: 2; minute: 0; second: 0; msg1: "北方海域"; msg2: "20 潜水艦哨戒任務" }

        ListElement { hour: 40; minute: 0; second: 0; msg1: "西方海域"; msg2: "21 通商破壊作戦" }
        ListElement { hour: 80; minute: 0; second: 0; msg1: "西方海域"; msg2: "22 敵母港空襲作戦" }
        ListElement { hour: 20; minute: 0; second: 0; msg1: "西方海域"; msg2: "23 潜水艦通商破壊作戦" }
    }

    //建造
    ListModel {
        id: constructionModel
        ListElement { hour: 0; minute: 18; second: 0; msg1: "駆逐艦"; msg2: "睦月型" }
        ListElement { hour: 0; minute: 20; second: 0; msg1: "駆逐艦"; msg2: "吹雪型 綾波型 暁型 初春型" }
        ListElement { hour: 0; minute: 22; second: 0; msg1: "駆逐艦"; msg2: "白露型 朝潮型" }
        ListElement { hour: 0; minute: 24; second: 0; msg1: "駆逐艦"; msg2: "陽炎型 雪風" }
        ListElement { hour: 0; minute: 30; second: 0; msg1: "駆逐艦"; msg2: "島風" }
        ListElement { hour: 1; minute: 0; second: 0; msg1: "軽巡洋艦"; msg2: "天龍型 球磨型 長良型 川内型" }
        ListElement { hour: 1; minute: 0; second: 0; msg1: "重巡洋艦"; msg2: "古鷹型 青葉型" }
        ListElement { hour: 1; minute: 15; second: 0; msg1: "軽巡洋艦"; msg2: "鬼怒 阿武隈" }
        ListElement { hour: 1; minute: 20; second: 0; msg1: "重巡洋艦"; msg2: "妙高型" }
        ListElement { hour: 1; minute: 22; second: 0; msg1: "軽巡洋艦"; msg2: "夕張" }
        ListElement { hour: 1; minute: 25; second: 0; msg1: "重巡洋艦"; msg2: "高雄型" }
        ListElement { hour: 1; minute: 30; second: 0; msg1: "重巡洋艦"; msg2: "最上型 利根型" }
        ListElement { hour: 2; minute: 0; second: 0; msg1: "軽空母"; msg2: "鳳翔" }
        ListElement { hour: 2; minute: 20; second: 0; msg1: "水上機母艦"; msg2: "千歳 千代田" }
        ListElement { hour: 2; minute: 40; second: 0; msg1: "軽空母"; msg2: "祥鳳 瑞鳳" }
        ListElement { hour: 2; minute: 50; second: 0; msg1: "軽空母"; msg2: "龍驤" }
        ListElement { hour: 3; minute: 0; second: 0; msg1: "軽空母"; msg2: "飛鷹 隼鷹" }
        ListElement { hour: 4; minute: 0; second: 0; msg1: "戦艦"; msg2: "金剛 比叡 榛名 霧島" }
        ListElement { hour: 4; minute: 10; second: 0; msg1: "正規空母"; msg2: "蒼龍 飛龍" }
        ListElement { hour: 4; minute: 20; second: 0; msg1: "戦艦"; msg2: "扶桑 山城" }
        ListElement { hour: 4; minute: 20; second: 0; msg1: "正規空母"; msg2: "加賀" }
        ListElement { hour: 4; minute: 30; second: 0; msg1: "戦艦"; msg2: "伊勢 日向" }
        ListElement { hour: 4; minute: 30; second: 0; msg1: "正規空母"; msg2: "赤城" }
        ListElement { hour: 5; minute: 0; second: 0; msg1: "戦艦"; msg2: "長門 陸奥" }
        ListElement { hour: 6; minute: 0; second: 0; msg1: "正規空母"; msg2: "翔鶴 瑞鶴" }
    }
}
