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
import QtQuick.Controls.Styles 1.0

Rectangle {
    id: root
    width: content.width * 1.1
    height: content.height * 1.3
    color: "#f0f0f0"

    property bool running: false
    property real setTime: 0        //指定時間
    property real startTime: 0      //開始時間
    property alias progress: progressBar.value       //進捗 0.0-1.0
    property real now: 0
    property int indexOffset: 0     //表示している番号をずらす

    signal started()
    signal stopped()
    signal setting()
    signal finished()

    //時間設定したらプログレスは0
    onSetTimeChanged: progress = 0

    function update(){
        if(running){
            now = (new Date() - 0)

            var r = setTime
            var n = root.now// new Date()
            var s = startTime
            if(r != 0)
                progress = (n - s) / r
            else
                progress = 0
            if((r - (n - s)) <= 0){
                //終わり
                progress = 1
                running = false
                root.finished()
            }
        }
    }

    function start(){
        if(setTime > 0){
            clear()
            progress = 0
            running = true
            started()
        }
    }
    function restart(){
        if(setTime > 0){
            running = true
        }
    }
    function stop(){
        clear()
        running = false;
        stopped()
    }

    function clear(){
        startTime = (new Date() - 0)
        now = startTime
    }

    Row {
        id: content
        anchors.centerIn: parent
        spacing: 10
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "%1 : ".arg(index + 1 + indexOffset)
            color: "darkGray"
            font.pointSize: 12
        }
        Column {
            Row {
                spacing: 5
                //残り時間
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: root.toRemainTimeString()
                    color: setTime > 0 ? "black" : "darkGray"
                    font.pointSize: 16
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "-"
                    font.pointSize: 10
                }
                //終了予定時刻
                Column {
                    anchors.bottom: parent.bottom
                    Text {
                        color: running ? "black" : "darkGray"
                        text: root.toEndDateString()
                        font.pointSize: 10
                    }
                    Text {
                        color: running ? "black" : "darkGray"
                        text: root.toEndTimeString()
                        font.pointSize: 10
                    }
                }
            }
            //進捗
            ProgressBar {
                id: progressBar
                anchors.horizontalCenter: parent.horizontalCenter
                width: 100
                height: 8
                style: ProgressBarStyle {
                    background: Rectangle {
                        radius: 2
                        color: "lightgray"
                        border.color: "gray"
                        border.width: 1
                    }
                    progress: Rectangle {
                        id: progressRect
                        color: "LightSkyBlue"
                        border.color: "steelblue"
                        states: [
                            State {
                                when: progressBar.value === 1
                                PropertyChanges {
                                    target: progressRect
                                    color: "LightCoral"
                                    border.color: "DarkRed"
                                }
                            }
                        ]
                    }
                }
            }
        }
        Row {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 3
            Button {
                width: 50
                text: running ? qsTr("Stop") : qsTr("Start")
                onClicked: {
                    if(running){
                        stop()
                    }else{
                        start()
                    }
                }
            }
            Button {
                width: 50
                text: qsTr("Set")
                onClicked: {
                    setting()
                }
            }
        }
    }
    //終わる予定日
    function toEndDateString(){
        var r = setTime
        var n = root.now
        var s = startTime
        if(running)
            return toStringDate(r + s - n)
        else
            return ""
    }
    //終わる予定時間
    function toEndTimeString(){
        var r = setTime
        var s = startTime
        if(running)
            return toStringTime(r + s)
        else
            return "00:00:00"
    }
    //残り時間の計算
    function toRemainTimeString(){
        var r = setTime
        var n = root.now// new Date()
        var s = startTime
        if(!running){
            return toStringUTCTime(r)
        }else if((r - (n - s)) < 0){
            return toStringUTCTime(0)
        }else{
            return toStringUTCTime(r - (n - s))
        }
    }
    //書式
    function toStringUTCTime(date){
        var d = new Date(date)
        var dd = d.getUTCDate() - 1
        var hh = d.getUTCHours() + dd * 24
        var mm = d.getUTCMinutes()
        var ss = d.getUTCSeconds()
        if(hh < 10)
            hh = "0" + hh
        else
            hh = "" + hh
        if(mm < 10)
            mm = "0" + mm
        else
            mm = "" + mm
        if(ss < 10)
            ss = "0" + ss
        else
            ss = "" + ss
        return "%1:%2:%3".arg(hh).arg(mm).arg(ss)
    }
    function toStringTime(date){
        var d = new Date(date)
        var hh = d.getHours()
        var mm = d.getMinutes()
        var ss = d.getSeconds()
        if(hh < 10)
            hh = "0" + hh
        else
            hh = "" + hh
        if(mm < 10)
            mm = "0" + mm
        else
            mm = "" + mm
        if(ss < 10)
            ss = "0" + ss
        else
            ss = "" + ss
        return "%1:%2:%3".arg(hh).arg(mm).arg(ss)
    }
    function toStringDate(date){
        var de = new Date(date + root.now)
        var YYYY = de.getFullYear() - 2000
        var MM = de.getMonth() + 1
        var DD = de.getDate()
        if(MM < 10)
            MM = "0" + MM
        if(DD < 10)
            DD = "0" + DD
        return ("%1/%2").arg(MM).arg(DD)
//        var d = new Date(date)
//        var dd = d.getUTCDate() - 1
//        var temp2 = new Date(root.now)
//        if((temp2.getHours() + d.getUTCHours() + 1) >= 24){
//            //日付超える
//            dd += 1
//        }
//        var ret = ""
//        switch(dd){
//        case 0:
//            ret = qsTr("Today")
//            break
//        case 1:
//            ret = qsTr("Yesterday")
//            break
//        default:
//            var de = new Date(date + root.now)
//            var YYYY = de.getFullYear() - 2000
//            var MM = de.getMonth() + 1
//            var DD = de.getDate()
//            if(MM < 10)
//                MM = "0" + MM
//            if(DD < 10)
//                DD = "0" + DD
////            ret = ("%1/%2/%3").arg(YYYY).arg(MM).arg(DD)
//            ret = ("%1/%2").arg(MM).arg(DD)
//            break;
//        }
//        return ret
    }
}
