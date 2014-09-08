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
import Qt.labs.folderlistmodel 1.0
import jp.relog.plugin.operatingsystem 1.0
import jp.relog.plugin.filelistmodel 1.0
import jp.relog.plugin.filelistfiltermodel 1.0

Rectangle {
    id: root
    width: 870
    height: 600
    color: "#f0f0f0"

    property int nextOperation: -1   // 0:tweet, 1:edit
    property string imagePath: ""

    OperatingSystem {
        id: os
    }

    Row {
        id: control
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 20
        anchors.bottomMargin: 5
        spacing: 10

        Button {
            width: 81
            height: 31
            text: qsTr("Select")
            onClicked: {
                nextOperation = 0
                Qt.quit()
            }
        }
        Button {
            width: 81
            height: 31
            text: qsTr("Edit")
            onClicked: {
                nextOperation = 1
                Qt.quit()
            }
        }
        Button {
            width: 81
            height: 31
            text: qsTr("Cancel")
            onClicked: {
                root.imagePath = ""
                Qt.quit()
            }
        }
        //        Button {
        //            width: 81
        //            height: 31
        //            text: qsTr("Delete")
        //            enabled: grid.currentIndex >= 0
        //            onClicked: {

        //            }
        //        }
    }

    ScrollView {
        id: scroll
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: control.top
        anchors.margins: 5

        ListView {
            id: list

            //ファイル一覧をモデルで取得
            model: FileListFilterModel {
                id: fileListFilterModel
                nameFilters: ["shiromusu_*.png", "shiromusu_*.jpg"]
                onNameFiltersChanged: folder = memoryPath
            }
            //個々のレイアウト
            delegate: Column {
                width: list.width
                spacing: 5

                Rectangle {
                    width: parent.width
                    height: text.paintedHeight * 2
//                    color: root.color
                    Text {
                        id: text
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        text: model.filterName
                        font.pixelSize: 16
                    }
                }

                ImageGrid {
                    contentWidth: parent.width
                    filter: [model.filter + ".png", model.filter + ".jpg"]
                    onFilterChanged: folder = memoryPath
                    onClicked: {
                        root.imagePath = filePath
                        viewImage.source = os.pathPrefix + filePath
                        view.xScale = 1
                    }
                    onDoubleClicked: {
                        root.imagePath = filePath
                        root.nextOperation = 0
                        Qt.quit()
                    }
                }
            }
        }
    }
    Rectangle {
        id: view
        anchors.fill: scroll
        color: "#aa000000"
        clip: true
        visible: xScale > 0
        property real xScale: 0
        Text {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 5
            text: qsTr("selected by double-click") + " / " + qsTr("close with right click")
            color: "white"
            font.pointSize: 10
        }
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
            onWheel: wheel.accepted = true
            onClicked: {
                if(mouse.button == Qt.RightButton)
                    view.xScale = 0
            }
            onDoubleClicked: {
                nextOperation = 0
                Qt.quit()
            }
        }
        Image {
            id: viewImage
            anchors.centerIn: parent
            width: view.width * 0.9
            height: view.height * 0.9
            fillMode: Image.PreserveAspectFit
            source: ""
            transform: Scale {
                origin.x: viewImage.width / 2
                origin.y: viewImage.height / 2
                xScale: view.xScale

                Behavior on xScale {
                    NumberAnimation { duration: 200 }
                }
            }
        }
    }
}
