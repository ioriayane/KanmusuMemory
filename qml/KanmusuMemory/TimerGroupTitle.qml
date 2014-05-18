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
import QtQuick 2.0
import QtQuick.Controls 1.0

Row {
    property alias caption: caption.text
    property alias itemClose: visiblityButton.itemClose

    spacing: 5

    Image {
        id: visiblityButton
        anchors.verticalCenter: parent.verticalCenter
        width: 16
        height: 16
        source: "images/triangle.png"
        rotation: visiblityButton.itemClose ? 0 : 90
        property bool itemClose: false
        Behavior on rotation {
            NumberAnimation {
                duration: 300
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: visiblityButton.itemClose = !visiblityButton.itemClose
        }
    }
    Text {
        id: caption
        text: qsTr("Title")
        font.pixelSize: 20
        MouseArea {
            anchors.fill: parent
            onClicked: visiblityButton.itemClose = !visiblityButton.itemClose
        }
    }
}
