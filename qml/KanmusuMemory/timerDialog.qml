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
import QtMultimedia 5.0

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

    Audio {
        id: sound
        volume: timerData.alarmSoundVolume
        source: timerData.alarmSoundPath
    }

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
                        onFinished: sound.play()
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
                        onFinished: sound.play()
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
                        onFinished: sound.play()
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
            text: qsTr("tweet a time-out")
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
            docking.itemAt(index).start()
            break;
        case 1: //遠征
            expedition.itemAt(index).stop()
            timerData.setTime(kind, index, settime)
            expedition.itemAt(index).start()
            break;
        case 2: //建造
            construction.itemAt(index).stop()
            timerData.setTime(kind, index, settime)
            construction.itemAt(index).start()
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
    property list<QtObject> expeditionModel: [
        QtObject { property int hour: 0; property int minute: 15; property int second: 0; property string msg1: qsTr("Guardian Office waters"); property string msg2: qsTr("1 Practice voyage"); property int page: 0 }
        , QtObject { property int hour: 0; property int minute: 30; property int second: 0; property string msg1: qsTr("Guardian Office waters"); property string msg2: qsTr("2 Long-distance voyage practice"); property int page: 0 }
        , QtObject { property int hour: 0; property int minute: 20; property int second: 0; property string msg1: qsTr("Guardian Office waters"); property string msg2: qsTr("3 Security mission"); property int page: 0 }
        , QtObject { property int hour: 0; property int minute: 50; property int second: 0; property string msg1: qsTr("Guardian Office waters"); property string msg2: qsTr("4 Antisubmarine security mission"); property int page: 0 }
        , QtObject { property int hour: 1; property int minute: 30; property int second: 0; property string msg1: qsTr("Guardian Office waters"); property string msg2: qsTr("5 Marine escort mission"); property int page: 0 }
        , QtObject { property int hour: 0; property int minute: 40; property int second: 0; property string msg1: qsTr("Guardian Office waters"); property string msg2: qsTr("6 Air defense target practice"); property int page: 0 }
        , QtObject { property int hour: 1; property int minute: 0; property int second: 0; property string msg1: qsTr("Guardian Office waters"); property string msg2: qsTr("7 Naval review rehearsal"); property int page: 0 }
        , QtObject { property int hour: 3; property int minute: 0; property int second: 0; property string msg1: qsTr("Guardian Office waters"); property string msg2: qsTr("8 Naval review"); property int page: 0 }

        , QtObject { property int hour: 4; property int minute: 0; property int second: 0; property string msg1: qsTr("Nansei Islands waters"); property string msg2: qsTr("9 Tanker escort mission"); property int page: 1 }
        , QtObject { property int hour: 1; property int minute: 30; property int second: 0; property string msg1: qsTr("Nansei Islands waters"); property string msg2: qsTr("10 Reconnaissance in force mission"); property int page: 1 }
        , QtObject { property int hour: 5; property int minute: 0; property int second: 0; property string msg1: qsTr("Nansei Islands waters"); property string msg2: qsTr("11 Bauxite transport mission"); property int page: 1 }
        , QtObject { property int hour: 8; property int minute: 0; property int second: 0; property string msg1: qsTr("Nansei Islands waters"); property string msg2: qsTr("12 Resource transport mission"); property int page: 1 }
        , QtObject { property int hour: 4; property int minute: 0; property int second: 0; property string msg1: qsTr("Nansei Islands waters"); property string msg2: qsTr("13 Rat transport creating"); property int page: 1 }
        , QtObject { property int hour: 6; property int minute: 0; property int second: 0; property string msg1: qsTr("Nansei Islands waters"); property string msg2: qsTr("14 Siege naval brigade withdrawal strategy"); property int page: 1 }
        , QtObject { property int hour: 12; property int minute: 0; property int second: 0; property string msg1: qsTr("Nansei Islands waters"); property string msg2: qsTr("15 Decoy task force support operations"); property int page: 1 }
        , QtObject { property int hour: 15; property int minute: 0; property int second: 0; property string msg1: qsTr("Nansei Islands waters"); property string msg2: qsTr("16 Decisive battle fleet assistance strategy"); property int page: 1 }

        , QtObject { property int hour: 0; property int minute: 45; property int second: 0; property string msg1: qsTr("Northern waters"); property string msg2: qsTr("17 Enemy territory reconnaissance operations"); property int page: 2 }
        , QtObject { property int hour: 5; property int minute: 0; property int second: 0; property string msg1: qsTr("Northern waters"); property string msg2: qsTr("18 Air transport operations"); property int page: 2 }
        , QtObject { property int hour: 6; property int minute: 0; property int second: 0; property string msg1: qsTr("Northern waters"); property string msg2: qsTr("19 Northern issue strategy"); property int page: 2 }
        , QtObject { property int hour: 2; property int minute: 0; property int second: 0; property string msg1: qsTr("Northern waters"); property string msg2: qsTr("20 Submarine patrol"); property int page: 2 }

        , QtObject { property int hour: 40; property int minute: 0; property int second: 0; property string msg1: qsTr("Western waters"); property string msg2: qsTr("21 Trade destruction strategy"); property int page: 3 }
        , QtObject { property int hour: 80; property int minute: 0; property int second: 0; property string msg1: qsTr("Western waters"); property string msg2: qsTr("22 Enemy air raid home port strategy"); property int page: 3 }
        , QtObject { property int hour: 20; property int minute: 0; property int second: 0; property string msg1: qsTr("Western waters"); property string msg2: qsTr("23 Submarine destruction trade strategy"); property int page: 3 }
    ]
//    ListModel {
//        id: expeditionModel
//        ListElement { hour: 0; minute: 15; second: 0; msg1: qsTr("Guardian Office waters"); msg2: qsTr("1 Practice voyage") }
//        ListElement { hour: 0; minute: 30; second: 0; msg1: qsTr("Guardian Office waters"); msg2: qsTr("2 Long-distance voyage practice") }
//        ListElement { hour: 0; minute: 20; second: 0; msg1: qsTr("Guardian Office waters"); msg2: qsTr("3 Security mission") }
//        ListElement { hour: 0; minute: 50; second: 0; msg1: qsTr("Guardian Office waters"); msg2: qsTr("4 Antisubmarine security mission") }
//        ListElement { hour: 1; minute: 30; second: 0; msg1: qsTr("Guardian Office waters"); msg2: qsTr("5 Marine escort mission") }
//        ListElement { hour: 0; minute: 40; second: 0; msg1: qsTr("Guardian Office waters"); msg2: qsTr("6 Air defense target practice") }
//        ListElement { hour: 1; minute: 0; second: 0; msg1: qsTr("Guardian Office waters"); msg2: qsTr("7 Naval review rehearsal") }
//        ListElement { hour: 3; minute: 0; second: 0; msg1: qsTr("Guardian Office waters"); msg2: qsTr("8 Naval review") }

//        ListElement { hour: 4; minute: 0; second: 0; msg1: qsTr("Nansei Islands waters"); msg2: qsTr("9 Tanker escort mission") }
//        ListElement { hour: 1; minute: 30; second: 0; msg1: qsTr("Nansei Islands waters"); msg2: qsTr("10 Reconnaissance in force mission") }
//        ListElement { hour: 5; minute: 0; second: 0; msg1: qsTr("Nansei Islands waters"); msg2: qsTr("11 Bauxite transport mission") }
//        ListElement { hour: 8; minute: 0; second: 0; msg1: qsTr("Nansei Islands waters"); msg2: qsTr("12 Resource transport mission") }
//        ListElement { hour: 4; minute: 0; second: 0; msg1: qsTr("Nansei Islands waters"); msg2: qsTr("13 Rat transport creating") }
//        ListElement { hour: 6; minute: 0; second: 0; msg1: qsTr("Nansei Islands waters"); msg2: qsTr("14 Siege naval brigade withdrawal strategy") }
//        ListElement { hour: 12; minute: 0; second: 0; msg1: qsTr("Nansei Islands waters"); msg2: qsTr("15 Decoy task force support operations") }
//        ListElement { hour: 15; minute: 0; second: 0; msg1: qsTr("Nansei Islands waters"); msg2: qsTr("16 Decisive battle fleet assistance strategy") }

//        ListElement { hour: 0; minute: 45; second: 0; msg1: qsTr("Northern waters"); msg2: qsTr("17 Enemy territory reconnaissance operations") }
//        ListElement { hour: 5; minute: 0; second: 0; msg1: qsTr("Northern waters"); msg2: qsTr("18 Air transport operations") }
//        ListElement { hour: 6; minute: 0; second: 0; msg1: qsTr("Northern waters"); msg2: qsTr("19 Northern issue strategy") }
//        ListElement { hour: 2; minute: 0; second: 0; msg1: qsTr("Northern waters"); msg2: qsTr("20 Submarine patrol") }

//        ListElement { hour: 40; minute: 0; second: 0; msg1: qsTr("Western waters"); msg2: qsTr("21 Trade destruction strategy") }
//        ListElement { hour: 80; minute: 0; second: 0; msg1: qsTr("Western waters"); msg2: qsTr("22 Enemy air raid home port strategy") }
//        ListElement { hour: 20; minute: 0; second: 0; msg1: qsTr("Western waters"); msg2: qsTr("23 Submarine destruction trade strategy") }
//    }

    //建造
    property list<QtObject> constructionModel: [
        QtObject { property int hour: 0; property int minute: 18; property int second: 0; property string msg1: qsTr("Destroyer"); property string msg2: qsTr("Mutsuki") }
        , QtObject { property int hour: 0; property int minute: 20; property int second: 0; property string msg1: qsTr("Destroyer"); property string msg2: qsTr("Fubuki Ayanami Akatsuki Hatsuharu") }
        , QtObject { property int hour: 0; property int minute: 22; property int second: 0; property string msg1: qsTr("Destroyer"); property string msg2: qsTr("Siratsuyu Asashio") }
        , QtObject { property int hour: 0; property int minute: 24; property int second: 0; property string msg1: qsTr("Destroyer"); property string msg2: qsTr("Kagero Yukikaze") }
        , QtObject { property int hour: 0; property int minute: 30; property int second: 0; property string msg1: qsTr("Destroyer"); property string msg2: qsTr("Simakaze") }
        , QtObject { property int hour: 1; property int minute: 0; property int second: 0; property string msg1: qsTr("Light cruiser"); property string msg2: qsTr("Tenryu Kuma Nagara Sendai") }
        , QtObject { property int hour: 1; property int minute: 0; property int second: 0; property string msg1: qsTr("Heavy cruiser"); property string msg2: qsTr("Kodaka Aoba") }
        , QtObject { property int hour: 1; property int minute: 15; property int second: 0; property string msg1: qsTr("Light cruiser"); property string msg2: qsTr("Kinu Abukuma") }
        , QtObject { property int hour: 1; property int minute: 20; property int second: 0; property string msg1: qsTr("Heavy cruiser"); property string msg2: qsTr("Myoko") }
        , QtObject { property int hour: 1; property int minute: 22; property int second: 0; property string msg1: qsTr("Light cruiser"); property string msg2: qsTr("Yubari") }
        , QtObject { property int hour: 1; property int minute: 25; property int second: 0; property string msg1: qsTr("Heavy cruiser"); property string msg2: qsTr("Takao") }
        , QtObject { property int hour: 1; property int minute: 30; property int second: 0; property string msg1: qsTr("Heavy cruiser"); property string msg2: qsTr("Mogami Tone") }
        , QtObject { property int hour: 2; property int minute: 0; property int second: 0; property string msg1: qsTr("Light aircraft carrier"); property string msg2: qsTr("Hosyo") }
        , QtObject { property int hour: 2; property int minute: 20; property int second: 0; property string msg1: qsTr("Seaplane carrier"); property string msg2: qsTr("Chitose Chiyoda") }
        , QtObject { property int hour: 2; property int minute: 40; property int second: 0; property string msg1: qsTr("Light aircraft carrier"); property string msg2: qsTr("Syoho Zuiho") }
        , QtObject { property int hour: 2; property int minute: 50; property int second: 0; property string msg1: qsTr("Light aircraft carrier"); property string msg2: qsTr("Ryujo") }
        , QtObject { property int hour: 3; property int minute: 0; property int second: 0; property string msg1: qsTr("Light aircraft carrier"); property string msg2: qsTr("Hiyo Junyo") }
        , QtObject { property int hour: 4; property int minute: 0; property int second: 0; property string msg1: qsTr("Battleship"); property string msg2: qsTr("Kongo Hie Haruna Kirishima") }
        , QtObject { property int hour: 4; property int minute: 10; property int second: 0; property string msg1: qsTr("Regular aircraft carrier"); property string msg2: qsTr("Soryu Hiryu") }
        , QtObject { property int hour: 4; property int minute: 20; property int second: 0; property string msg1: qsTr("Battleship"); property string msg2: qsTr("Fuso Yamashiro") }
        , QtObject { property int hour: 4; property int minute: 20; property int second: 0; property string msg1: qsTr("Regular aircraft carrier"); property string msg2: qsTr("Kaga") }
        , QtObject { property int hour: 4; property int minute: 30; property int second: 0; property string msg1: qsTr("Battleship"); property string msg2: qsTr("Ise Hyuga") }
        , QtObject { property int hour: 4; property int minute: 30; property int second: 0; property string msg1: qsTr("Regular aircraft carrier"); property string msg2: qsTr("Akagi") }
        , QtObject { property int hour: 5; property int minute: 0; property int second: 0; property string msg1: qsTr("Battleship"); property string msg2: qsTr("Nagato Mutsu") }
        , QtObject { property int hour: 6; property int minute: 0; property int second: 0; property string msg1: qsTr("Regular aircraft carrier"); property string msg2: qsTr("Syokaku Zuikaku") }
    ]
//    ListModel {
//        id: constructionModel
//        ListElement { hour: 0; minute: 18; second: 0; msg1: qsTr("Destroyer"); msg2: qsTr("Mutsuki") }
//        ListElement { hour: 0; minute: 20; second: 0; msg1: qsTr("Destroyer"); msg2: qsTr("Fubuki Ayanami Akatsuki Hatsuharu") }
//        ListElement { hour: 0; minute: 22; second: 0; msg1: qsTr("Destroyer"); msg2: qsTr("Siratsuyu Asashio") }
//        ListElement { hour: 0; minute: 24; second: 0; msg1: qsTr("Destroyer"); msg2: qsTr("Kagero Yukikaze") }
//        ListElement { hour: 0; minute: 30; second: 0; msg1: qsTr("Destroyer"); msg2: qsTr("Simakaze") }
//        ListElement { hour: 1; minute: 0; second: 0; msg1: qsTr("Light cruiser"); msg2: qsTr("Tenryu Kuma Nagara Sendai") }
//        ListElement { hour: 1; minute: 0; second: 0; msg1: qsTr("Heavy cruiser"); msg2: qsTr("Kodaka Aoba") }
//        ListElement { hour: 1; minute: 15; second: 0; msg1: qsTr("Light cruiser"); msg2: qsTr("Kinu Abukuma") }
//        ListElement { hour: 1; minute: 20; second: 0; msg1: qsTr("Heavy cruiser"); msg2: qsTr("Myoko") }
//        ListElement { hour: 1; minute: 22; second: 0; msg1: qsTr("Light cruiser"); msg2: qsTr("Yubari") }
//        ListElement { hour: 1; minute: 25; second: 0; msg1: qsTr("Heavy cruiser"); msg2: qsTr("Takao") }
//        ListElement { hour: 1; minute: 30; second: 0; msg1: qsTr("Heavy cruiser"); msg2: qsTr("Mogami Tone") }
//        ListElement { hour: 2; minute: 0; second: 0; msg1: qsTr("Light aircraft carrier"); msg2: qsTr("Hosyo") }
//        ListElement { hour: 2; minute: 20; second: 0; msg1: qsTr("Seaplane carrier"); msg2: qsTr("Chitose Chiyoda") }
//        ListElement { hour: 2; minute: 40; second: 0; msg1: qsTr("Light aircraft carrier"); msg2: qsTr("Syoho Zuiho") }
//        ListElement { hour: 2; minute: 50; second: 0; msg1: qsTr("Light aircraft carrier"); msg2: qsTr("Ryujo") }
//        ListElement { hour: 3; minute: 0; second: 0; msg1: qsTr("Light aircraft carrier"); msg2: qsTr("Hiyo Junyo") }
//        ListElement { hour: 4; minute: 0; second: 0; msg1: qsTr("Battleship"); msg2: qsTr("Kongo Hie Haruna Kirishima") }
//        ListElement { hour: 4; minute: 10; second: 0; msg1: qsTr("Regular aircraft carrier"); msg2: qsTr("Soryu Hiryu") }
//        ListElement { hour: 4; minute: 20; second: 0; msg1: qsTr("Battleship"); msg2: qsTr("Fuso Yamashiro") }
//        ListElement { hour: 4; minute: 20; second: 0; msg1: qsTr("Regular aircraft carrier"); msg2: qsTr("Kaga") }
//        ListElement { hour: 4; minute: 30; second: 0; msg1: qsTr("Battleship"); msg2: qsTr("Ise Hyuga") }
//        ListElement { hour: 4; minute: 30; second: 0; msg1: qsTr("Regular aircraft carrier"); msg2: qsTr("Akagi") }
//        ListElement { hour: 5; minute: 0; second: 0; msg1: qsTr("Battleship"); msg2: qsTr("Nagato Mutsu") }
//        ListElement { hour: 6; minute: 0; second: 0; msg1: qsTr("Regular aircraft carrier"); msg2: qsTr("Syokaku Zuikaku") }
//    }
}
