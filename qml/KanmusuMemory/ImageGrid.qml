import QtQuick 2.0
import jp.relog.plugin.filelistmodel 1.0

Grid {
    id: root
    columns: contentWidth / 200
    spacing: 5

    property real contentWidth: 200
    property string folder: ""
    property var filter: []
    property bool multiSelectMode: false
    property var selectedFiles: []

    signal clicked(string filePath, bool ctrl)
    signal doubleClicked(string filePath)

    Repeater {
        id: repeater
        model: FileListModel {
            id: folderlist
            nameFilters: root.filter
            onNameFiltersChanged: folder = root.folder
        }

        delegate: Image {
            id: image
            width: 200
            height: 120
            fillMode: Image.PreserveAspectFit
            source: model.filePath === undefined ? "" : "image://thumbnail/" + model.filePath
            property int selectedIndex: 0

            //選択中
            Rectangle {
                anchors.fill: parent
                color:"#00000000"
                border.color: "#ff8844"
                border.width: 5
                visible: checkSelected(model.filePath) && multiSelectMode

                Rectangle {
                    anchors.top: parent.top
                    anchors.right: parent.right
                    width: 18
                    height: 18
                    color: parent.border.color
                    Text {
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        color: "white"
//                        font.pointSize: 12
                        font.pixelSize: 15
                        text: selectedIndex
                    }
                }
            }

            //読み込み中
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

            //クリックイベント
            MouseArea {
                id: mouse
                anchors.fill: parent

                onClicked: {
                    if(mouse.modifiers & Qt.ControlModifier){
                        root.clicked(model.filePath, true)
                    }else{
                        root.clicked(model.filePath, false)
                    }
                }
                onDoubleClicked: root.doubleClicked(model.filePath)
            }

            function checkSelected(path){
                var ret = false;
                if(multiSelectMode){
                    for(var i=0; i<selectedFiles.length; i++){
                        if(path === selectedFiles[i]){
                            ret = true;
                            selectedIndex = i+1
                        }
                    }
                }
                return ret;
            }
        }
    }

}
