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
    color: "#f0f0f0"
    opacity: 0

    property int kind: 0        // 0:入渠 , 1:遠征, 2:建造
    property int index: 0       // それぞれの何番目か

    property alias spinHour: spinHour.value
    property alias spinMinute: spinMinute.value
    property alias spinSecond: spinSecond.value

    property alias model: list.model

    signal accepted(int kind, int index, int hour, int minute, int second)
    signal close()

    Keys.onEscapePressed: root.close()

    Behavior on x {
        NumberAnimation {
            duration: 200
        }
    }
    Behavior on opacity {
        NumberAnimation {
            duration: 200
        }
    }

    Item {
        anchors.fill: parent
        anchors.margins: 5
        Row {
            id: control
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 5
            SpinBox {
                id: spinHour
                width: 50
                height: 30
                minimumValue: 0
                font.pointSize: 14
                Keys.onEscapePressed: root.close()
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: ":"
                font.pointSize: 14
            }
            SpinBox {
                id: spinMinute
                width: 50
                height: 30
                maximumValue: 59
                minimumValue: 0
                stepSize: 10
                font.pointSize: 14
                Keys.onEscapePressed: root.close()
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: ":"
                font.pointSize: 14
            }
            SpinBox {
                id: spinSecond
                width: 50
                height: 30
                maximumValue: 59
                minimumValue: 0
                stepSize: 10
                font.pointSize: 14
                Keys.onEscapePressed: root.close()
            }
            Button {
                width: 60
                height: 30
                text: qsTr("Set")
                onClicked: {
                    root.accepted(root.kind, root.index
                                      , spinHour.value, spinMinute.value, spinSecond.value)
                    root.close()
                }
                Keys.onEscapePressed: root.close()
            }
            Button {
                width: 60
                height: 30
                text: qsTr("Cancel")
                onClicked: root.close()
                Keys.onEscapePressed: root.close()
            }
        }
        ScrollView {
            anchors.top: control.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            ListView {
                id: list
                anchors.fill: parent
                delegate: MouseArea {
                    width: parent.width
                    height: 40
                    hoverEnabled: true
                    Rectangle {
                        anchors.fill: parent
                        opacity: parent.containsMouse ? 1 : 0
                        color: "#440000ff"
                    }
                    Row {
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 10
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: root.toStringTime(model.hour, model.minute, model.second)
                            font.pointSize: 16
                        }
                        Column {
                            spacing: 1
                            Text {
                                text: model.msg1
                            }
                            Text {
                                text: model.msg2
                            }
                        }
                    }

                    onClicked: {
                        spinHour.value = model.hour
                        spinMinute.value = model.minute
                        spinSecond.value = model.second
                    }
                    onDoubleClicked: {
                        root.accepted(root.kind, root.index
                                          , spinHour.value, spinMinute.value, spinSecond.value)
                        root.close()
                    }
                }
            }
        }
    }

    function toStringTime(hh, mm, ss){
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
        return "%1:%2".arg(hh).arg(mm)
    }



}
