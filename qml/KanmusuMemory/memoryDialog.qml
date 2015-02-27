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
    property bool multiSelectMode: false
    property var selectedFiles: []

    OperatingSystem {
        id: os
    }

    //複数選択のメッセージ
    Text {
        id: multiSelectMessage
        anchors.top: control.top
        anchors.left: parent.left
        anchors.bottom: control.bottom
        anchors.leftMargin: 20
        verticalAlignment: Text.AlignVCenter
        color: "darkgray"
        text: qsTr("Select multiple images by Ctrl+Click.")

        states: [
            State {
                when: root.multiSelectMode
                PropertyChanges {
                    target: multiSelectMessage
                    color: "black"
                    text: qsTr("Selected images %1/%2.").arg(root.selectedFiles.length).arg(4)
                }
            }
        ]
    }

    //下のボタンとかエリア
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
            enabled: view.visible || selectedFiles.length > 0
            text: qsTr("Select")
            onClicked: {
                nextOperation = 0
                Qt.quit()
            }
        }
        Button {
            width: 81
            height: 31
            enabled: view.visible
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
                root.selectedFiles = []
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
                nameFilters: ["kanmusu_*.png", "kanmusu_*.jpg"]
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
                    multiSelectMode: root.multiSelectMode
                    selectedFiles: root.selectedFiles
                    onClicked: {
                        if(ctrl){
                            //複数選択モード
                            var temp = []
                            var exist = false
                            for(var i=0; i<root.selectedFiles.length; i++){
                                if(root.selectedFiles[i] === filePath){
                                    //既にある場合は消す
                                    exist = true
                                }else{
                                    temp.push(root.selectedFiles[i])
                                }
                            }
                            if(!exist && root.selectedFiles.length < 4){
                                temp.push(filePath)
                            }
                            root.selectedFiles = temp
                            //↑これしないと変更シグナルが飛ばないから
                            if(root.selectedFiles.length > 0){
                                root.imagePath = root.selectedFiles[0]
                            }
                            root.multiSelectMode = true
                            //console.debug("selectedCount:%1".arg(root.selectedFiles.length))
                        }else if(root.multiSelectMode){
                            root.imagePath = ""
                            root.selectedFiles = []
                            root.multiSelectMode = false
                        }else{
                            root.imagePath = filePath
                            root.selectedFiles = [filePath]
                            viewImage.source = os.pathPrefix + filePath
                            view.xScale = 1
                            root.multiSelectMode = false
                        }
                    }
                    onDoubleClicked: {
                        root.imagePath = filePath
                        root.selectedFiles = [filePath]
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
                if(mouse.button == Qt.RightButton){
                    root.selectedFiles = [] //クリア
                    view.xScale = 0
                }
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
