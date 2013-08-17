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

Rectangle {
    id: root
    width: 860
    height: 600
    color: "#f0f0f0"

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
            enabled: grid.currentIndex >= 0
            onClicked: {
                memoryData.imagePath = root.imagePath
                Qt.quit()
            }
        }
        Button {
            width: 81
            height: 31
            text: qsTr("Cancel")
            onClicked: {
                memoryData.imagePath = ""
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

        GridView {
            id: grid
            cellWidth: 200 + 6
            cellHeight: 120 + 6

            //ファイル一覧をモデルで取得
            model: FolderListModel {
                id: folderlist
                folder: os.pathPrefix + memoryData.memoryPath
                showDirs: false                                 //ディレクトリは非表示
                sortField: FolderListModel.Name                 //ファイル名でソート
                sortReversed: true                              //新しいモノが上
                nameFilters: "kanmusu_*.png"                    //フィルタ
            }
            //個々のレイアウト
            delegate: MouseArea {
                id: mouse
                width: image.width + 10
                height: image.height + 10
                hoverEnabled: true
                Image {
                    id: image
                    anchors.centerIn: parent
                    width: 200
                    height: 120
                    fillMode: Image.PreserveAspectCrop
                    source: "image://thumbnail/" + model.filePath

                    Text {
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 5
                        anchors.left: parent.left
                        anchors.right: parent.right
                        horizontalAlignment: Text.AlignRight
                        visible: mouse.containsMouse
                        text: model.fileModified
                        Rectangle {
                            anchors.centerIn: parent
                            width: parent.width
                            height: parent.height * 1.2
                            z: -1
                            opacity: 0.5
                        }
                    }

                    Image {
                        id: spin
                        anchors.centerIn: parent
                        source: "images/spinner.png"
                        visible: loading
                        property bool loading: image.status != Image.Ready

                        NumberAnimation on rotation {
                            from: 0
                            to: 360
                            duration: 1500
                            loops: Animation.Infinite
                            running: spin.loading
                        }
                    }
                }
                onClicked: {
                    grid.currentIndex = index
                    root.imagePath = model.filePath
                    viewImage.source = os.pathPrefix + model.filePath
                    view.xScale = 1
                }
                onDoubleClicked: {
                    memoryData.imagePath = model.filePath
                    Qt.quit()
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
                memoryData.imagePath = root.imagePath
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
