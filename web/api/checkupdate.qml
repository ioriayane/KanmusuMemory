import Silk.JSON 1.0
import QtQml 2.0
import me.qtquick.Database 0.1

Json {
    id: root

    object: { "update-exist": updateExist
            , "download-url": downloadUrl
            , "webpage-url": webpageUrl
            , "new-version": newVersion
    }

    UpdateInfo {
        id: info
    }
    
    property bool updateExist: false
    property string downloadUrl: info.downloadUrl[os]
    property string webpageUrl: info.webpageUrl
    property string newVersion: info.version

    property string version: ""
    property int os: 0
    property string lang: ""
    
    // OS number define
    // 0: Windows(32bit)
    // 1: Windows(64bit)
    // 2: Mac OS X
    // 3: Linux(32bit)
    // 4: Linux(64bit)

    Component.onCompleted: {
        var params
        var items
        
        // Parse User Information
        params = http.query.split('+')
        for(var i=0; i<params.length; i++){
            items = params[i].split('=')
            if(items[0] == 'os'){
                os = 0 + items[1]
                if(os > 4){
                    os = 0
                }
            }else if(items[0] == 'version'){
                version = items[1]
            }else if(items[0] == 'lang'){
                lang = items[1]
            }else{
            }
        }

        // Check Update exist
        if(newVersion > version){
            //exist
            updateExist = true
        }else{
            updateExist = false
        }


        // Insert User Information        
        db.tableModel.select = false;
        if(db.transaction()){
            db.tableModel.insert({"checkTime": (new Date())
                               , "version": root.version
                               , "os": root.os
                               , "lang": root.lang
                               , "userAgent": http.requestHeader["user-agent"]
                           })
            if(!db.commit()) db.rollback()
        }
        db.tableModel.select = true;
    }
    
    // Database Setting
    KanmemoDatabase {
        id: db        
    }
}

