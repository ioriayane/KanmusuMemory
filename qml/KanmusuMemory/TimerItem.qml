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
    width: content.width * 1.1
    height: content.height * 1.3
    color: "#f0f0f0"

    property bool running: false
    property real setTime: 0        //指定時間
    property real startTime: 0      //開始時間
    property real progress: 0       //進捗 0.0-1.0
    property real now: 0

    signal started()
    signal stopped()
    signal setting()
    signal finished()

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
                progress = 100
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
        Column {
            Row {
                spacing: 5
                //残り時間
                Text {
                    text: root.toRemainTimeString()
                    color: setTime > 0 ? "black" : "darkGray"
                    font.pointSize: 16
                }
                Text {
                    anchors.bottom: parent.bottom
                    text: "-"
                    font.pointSize: 10
                }
                //終了予定時刻
                Text {
                    anchors.bottom: parent.bottom
                    color: running ? "black" : "darkGray"
                    text: root.toEndTimeString()
                    font.pointSize: 12
                }
            }
            //進捗
            ProgressBar {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 100
                height: 8
                value: root.progress
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
        var hh = d.getUTCHours()
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
}
