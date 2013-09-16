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

Rectangle {
    id: root
//    color: "black"

    property int mode: 0    //0:どろー, 1:トリミング（エリア表示）, 2:トリミング（ドラッグ）, 3:エフェクト

    property string tempPath: ""
    property string tempPathNP: ""
    property string editPath: ""

    property rect cursor: Qt.rect(0, 0, image.width, image.height)

    Component.onCompleted: {
        root.cursor = Qt.rect(0, 0, image.width, image.height)

        canvas.loadImageUrl = root.tempPath
        canvas.loadImage(canvas.loadImageUrl)
    }

    function save(){
        canvas.save(editPath)
    }

    Image {
        id: image
        anchors.centerIn: parent
        source: root.tempPath
        visible: false
        opacity: 0.2
        cache: false
    }

    Canvas {
        id: canvas
        anchors.centerIn: image
        width: image.width
        height: image.height

        renderTarget: Canvas.Image          //描画先を指定
        renderStrategy: Canvas.Immediate    //描画スレッドの指定
        antialiasing: true                  //描画を滑らかにする

        property string loadImageUrl: ""    //読み込み用

        //画像の読み込み完了
        onImageLoaded: requestPaint()

        //描画処理
        onPaint: {
            var ctx = canvas.getContext("2d")
            //状態保存
            ctx.save()
            //クリア
            ctx.clearRect(0, 0, canvas.width, canvas.height)
            //画像の描画
            if(isImageLoaded(loadImageUrl)){
                ctx.drawImage(loadImageUrl
                              , root.cursor.x                     //src x
                              , root.cursor.y                     //src y
                              , root.cursor.width
                              , root.cursor.height
                              , 0
                              , 0
                              , root.cursor.width                 //dest w
                              , root.cursor.height)               //dest h
            }

            //状態復帰
            ctx.restore()
        }
    }

    //トリミングのカーソル
    ClipTool {
        id: clip
        anchors.fill: parent
        visible: false

        onVisibleChanged: setStartRange()
        onPositionChanged: canvas.requestPaint()

        onAccepted: {
            //clipの位置は画面全体の相対位置
            //rootは前回のクリップ位置で、それをつかってずらす。（トリミングのトリミング時に必要）
            root.cursor = Qt.rect(clip.selectRange.x + root.cursor.x
                                  , clip.selectRange.y + root.cursor.y
                                  , clip.selectRange.width
                                  , clip.selectRange.height)
            //キャンバスのサイズ調整
            canvas.width = clip.cursor.width
            canvas.height = clip.cursor.height
            canvas.requestPaint()
            //モードを戻す
            root.mode = 0
//            console.debug("save:" + root.cursor)
        }
        onCanceled: {
            //クリップ位置をリセット
            root.cursor = root.cursor
            canvas.requestPaint()
            root.mode = 0
        }

        function setStartRange(){
            if(visible){
                if(root.mode == 1){
                    //初期位置を決める
                    var x = canvas.x
                    var y = canvas.y
                    var width = canvas.width
                    var height = canvas.height
                    //カーソルの初期位置
                    if(width > 400 && height > 400){
                        clip.cursor = Qt.rect(x + 100, y + 100, width - 200, height - 200)
                    }else{
                        clip.cursor = Qt.rect(x, y, width, height)
                    }
                    availableRange = Qt.rect(canvas.x, canvas.y, canvas.width, canvas.height)

//                    console.debug("area load:" + clip.cursor)
                }else if(root.mode == 2){
                    clip.cursor = Qt.rect(0, 0, 1, 1)
                    availableRange = Qt.rect(canvas.x, canvas.y, canvas.width, canvas.height)

//                    console.debug("drag load:" + clip.cursor)
                }
            }
        }
    }

    states: [
        State { // トリミング（領域固定）
            when: root.mode == 1
            PropertyChanges {
                target: clip
                visible: true
                freeDrag: false
            }
        }
        , State { // トリミング（ドラッグ）
            when: root.mode == 2
            PropertyChanges {
                target: clip
                visible: true
                freeDrag: true
            }
        }
        , State {   //エフェクト
            when: root.mode == 2
        }

    ]
}
