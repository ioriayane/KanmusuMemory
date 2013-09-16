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
import jp.relog.plugin.operatingsystem 1.0

Rectangle {
    id: root
    width: 860
    height: 600
    color: "#f0f0f0"

    property int nextOperation: -1

    Component.onCompleted: imageEdit.mode = 2

    OperatingSystem {
        id: os
    }

    Row {
        id: editMode
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 20
        anchors.bottomMargin: 5
        spacing: 10

//        Button {
//            width: 81
//            height: 31
//            text: qsTr("Draw")
//            onClicked: {
//                imageEdit.mode = 0
//            }
//        }
        Button {
            width: 81
            height: 31
            text: qsTr("Crop")
            onClicked: {
                imageEdit.mode = 2
            }
        }
//        Button {
//            width: 81
//            height: 31
//            text: qsTr("Effects")
//            onClicked: {
//                imageEdit.mode = 2
//            }
//        }
    }

    Row {
        id: control
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 20
        anchors.bottomMargin: 5
        spacing: 10

        Button {
//            width: 81
            height: 31
            text: qsTr("Save and Tweet")
            onClicked: {
                imageEdit.save()
                nextOperation = 0;
                console.debug("save:" + editImagePath)
                Qt.quit()
            }
        }
        Button {
            width: 81
            height: 31
            text: qsTr("Save")
            onClicked: {
                imageEdit.save()
                nextOperation = 1;
                Qt.quit()
            }
        }
        Button {
            width: 81
            height: 31
            text: qsTr("Cancel")
            onClicked: {
                nextOperation = -1;
                Qt.quit()
            }
        }
    }

    ImageEditView {
        id: imageEdit
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: control.top
        anchors.bottomMargin: 5

        tempPath: os.pathPrefix + tempSourceImagePath
        tempPathNP: tempSourceImagePath
        editPath: editImagePath
    }
}
