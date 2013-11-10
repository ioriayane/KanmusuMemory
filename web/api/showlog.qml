import Silk.HTML 5.0
import Silk.Utils 1.0
import QtQml 2.0
import me.qtquick.Database 0.1

Html {
    id: root
    
    property variant osName: [
        "0:Windows(32bit)"
        , "1:Windows(64bit)"
        , "2:Mac OS X"
        , "3:Linux(32bit)"
        , "4:Linux(64bit)"]

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
                            Td { text: model.os > 4 ? "unkown" : osName[model.os] }
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
    }
    
    // Database Setting
    KanmemoDatabase {
        id: db
    }
}

