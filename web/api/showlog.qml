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
import Silk.HTML 5.0
import Silk.Utils 1.0
import QtQml 2.0
import me.qtquick.Database 0.1

Html {
    id: root
    
    property variant osName: [
        "0:Windows(32bit)"
        , "1:Windows(64bit)"
        , "2:Mac OS X(32bit)"
        , "3:Mac OS X(64bit)"
        , "4:Linux(32bit)"
        , "5:Linux(64bit)"]

    Head {
        Meta { charset: "utf-8" }
        Title { text: "KanmusuMemory Update check History" }
    }
    
    Body {
        H1 { text: "Update check history" }
        
        Table {
            THead {
                Tr {
                    Th { text: 'key' }
                    Th { text: 'date' }
                    Th { text: 'version' }
                    Th { text: 'code' }
                    Th { text: 'os' }
                    Th { text: 'lang' }
                    Th { text: 'ip' }
                    Th { text: 'agent' }
                }
            }
            TBody {
                Repeater {
                    model: db.tableModel
                    Component {
                        Tr {
                            Td { text: model.key }
                            Td { text: model.checkTime }
                            Td { text: model.version }
                            Td { text: model.versionCode }
                            Td { text: model.os > 5 ? "%1:unkown".arg(model.os) : osName[model.os] }
                            Td { text: model.lang }
                            Td { text: model.remoteAddress }
                            Td { text: model.userAgent }
                        }
                    }
                }
            }
        }
    }
    
    Component.onCompleted: {
        var d = new Date() - (24 * 3600 * 1000)
        d = new Date(d)
        var mm = d.getMonth() + 1
        var dd = d.getDate()
        var hh = d.getHours()
        var m = d.getMinutes()
        if(mm < 10) { mm = "0" + mm }
        if(dd < 10) { dd = "0" + dd }
        if(hh < 10) { hh = "0" + hh }
        if(m < 10) { m = "0" + m }
        db.tableModel.condition = "checkTime > '%1-%2-%3T%4:%5:00'".arg(d.getFullYear()).arg(mm).arg(dd).arg(hh).arg(m)
        db.tableModel.select = true
    }
    
    // Database Setting
    KanmemoDatabase {
        id: db
    }
}

