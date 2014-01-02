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
    width: contentWidth * viewRatio * 1.1 * 2 + grid.x * 2 + grid.spacing * (columns - 1)
    height: 640
    color: "#f0f0f0"

    property int nextOperation: -1   // 0:結合 -1:キャンセル
    //横に並べる数
    property int columns: 2
    //最大の数
    property int max: 6
    //キャプチャする画像の情報
    property real contentX: 0
    property real contentY: 0
    property real contentWidth: 600
    property real contentHeight: 320
    //表示比率
    property real viewRatio: 0.4
    //キャプチャした画像のリスト
    property variant imageList: []
    //メッセージリスト
    property variant messageList: []

    onMessageListChanged: {
        msgModel.clear()
        for(var i=0; i<messageList.length; i++){
            msgModel.append({"text": messageList[i]})
        }
    }

    function clear(){
        imageList = []
        sourceList.clear()
        sourceList.append({"source": ""})
    }

    OperatingSystem {
        id: os
    }

    Column {
        id: message
        Repeater {
            model: ListModel{ id: msgModel }
            delegate: Text {
                width: root.width
                text: model.text
                wrapMode: Text.WordWrap
                font.pixelSize: 16
            }
        }
    }
    ScrollView {
        id: scrollview
        anchors.top: message.bottom
        anchors.topMargin: 5
        anchors.bottom: control.top
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.right: parent.right
        Grid {
            id: grid
            columns: root.columns
            spacing: 2
            Repeater {
                id: repeater
                model: ListModel{
                    id: sourceList
                    ListElement{ source: "" }
                }
                delegate: CaptureControl {
                    //キャプチャした画像のサイズ
                    contentWidth: root.contentWidth
                    contentHeight: root.contentHeight
                    //表示比率
                    viewRatio: root.viewRatio
                    //クリック
                    onCapture: {
//                        console.debug("%1 %2 %3 %4".arg(root.contentX)
//                                      .arg(root.contentY)
//                                      .arg(root.contentWidth)
//                                      .arg(root.contentHeight))
                        //キャプチャして画像のパスを取得する
                        source = os.pathPrefix + CaptureInterface.captureGame(root.contentX
                                                                              , root.contentY
                                                                              , root.contentWidth
                                                                              , root.contentHeight)
                        //画像の登録
                        var list = []
                        for(var i=0; i<imageList.length; i++){
                            list.push(imageList[i])
                        }
                        if(list.length == index){
                            list.push(source.replace(os.pathPrefix, ""))
                        }else{
                            list[index] = source.replace(os.pathPrefix, "")
                        }
                        imageList = list;

                        //コントロールの追加
                        if(repeater.count == (index + 1)        //最後か
                                && repeater.count < root.max    //制限内か
                                ){
                            //最後のクリックしたら追加
                            sourceList.append({"source": ""})
                        }
                    }
                }

            }
        }
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
            text: qsTr("Combine")
            enabled: repeater.count > 0
            onClicked: {
                nextOperation = 0
                Qt.quit()
            }
        }
        Button {
            width: 81
            height: 31
            text: qsTr("Cancel")
            onClicked: {
                nextOperation = 1
                Qt.quit()
            }
        }
    }


}
