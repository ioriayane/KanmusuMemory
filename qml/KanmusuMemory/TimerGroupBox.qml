/*
 * Copyright 2013-2014 KanMemo Project.
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

GroupBox {
    id: root

    property bool itemClose: false

    //ボックスをたたむ
    transform: Scale {
        id: scale
        origin.y: 0
        yScale: 1
    }
    states: State {
        when: itemClose
        PropertyChanges { target: root; height: 1 }
        PropertyChanges { target: scale; yScale: 0  }
    }
    transitions: Transition {
        NumberAnimation { target: root; property: "height"; duration: 200 }
        NumberAnimation { target: scale; property: "yScale"; duration: 200 }
    }
}
