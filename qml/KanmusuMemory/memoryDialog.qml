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
                showDirs: false
                //フィルタ作成（関数の中も評価してくれる）
                nameFilters: "kanmusu_*.png"
            }
            //選択アイテムのハイライト用レイアウト
            highlight: Rectangle {
                color: "#78aee5"
                border.color: "#78aee5"
                border.width: 1
                radius: 2
                antialiasing: true
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
                }
                onDoubleClicked: {
                    memoryData.imagePath = model.filePath
                    Qt.quit()
                }
            }
        }
    }
}
