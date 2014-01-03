import QtQuick 2.0
import jp.relog.plugin.filelistmodel 1.0

Grid {
    id: root
    columns: contentWidth / 200
    spacing: 5

    property real contentWidth: 200
    property string folder: ""
    property variant filter: []

    signal clicked(string filePath)
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

                onClicked: root.clicked(model.filePath)
                onDoubleClicked: root.doubleClicked(model.filePath)
            }
        }
    }

}
