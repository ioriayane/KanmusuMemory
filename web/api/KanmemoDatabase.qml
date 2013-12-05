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
import me.qtquick.Database 0.1

Database {
    id: db
    connectionName: 'KanmusuMemory'
    type: "QSQLITE"
    databaseName: onMemory ? ":memory:" : "/home/iori/silk-build/share/silk/db/KanmusuMemory.db"

    property bool onMemory: false
    property alias tableModel: history
    
    TableModel {
        id: history
        tableName: 'UpdateCheckHistory'
        select: false
        primaryKey: 'key'
        property int key
        property date checkTime
        property string version
        property int versionCode
        property int os
        property string lang
        property string userAgent
        property string remoteAddress
    }
}

