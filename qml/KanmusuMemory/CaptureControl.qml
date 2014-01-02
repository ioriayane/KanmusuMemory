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
    width: control.width * 1.1
    height: control.height * 1.1
    color: "#00ffffff"
    border.color: "gray"
    border.width: 1

    //キャプチャした画像のサイズ
    property real contentWidth: 800
    property real contentHeight: 640
    //表示比率
    property real viewRatio: 0.2
    //キャプチャした画像のパス
    property string source: ""
    //ボタンをクリック
    signal capture()

    Column {
        id: control
        anchors.centerIn: parent
        spacing: 5
        Image {
            id: image
            width: contentWidth * viewRatio
            height: contentHeight * viewRatio
            source: "images/noimage.png"
            fillMode: Image.PreserveAspectFit
            smooth: true
            states: State {
                when: root.source.length > 0
                PropertyChanges {
                    target: image
                    source: root.source
                }
            }
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Capture")
            onClicked: root.capture()
        }
    }
}
