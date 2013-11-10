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
        select: true
        primaryKey: 'key'
        property int key
        property date checkTime
        property string version
        property int os
        property string lang
        property string userAgent
    }
}

