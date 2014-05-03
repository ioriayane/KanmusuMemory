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
import Silk.JSON 1.0
import QtQml 2.0
import me.qtquick.Database 0.1

Json {
    id: root

    object: { "update-exist": updateExist
        , "download-url": downloadUrl
        , "webpage-url": webpageUrl
        , "new-version": newVersion
        , "new-version-code": newVersionCode
        , "message" : message
        , "last-favorite" : lastFavoriteData
        , "last-timer" : lastTimerSelectGuide
    }

    UpdateInfo {
        id: info
    }
    
    property bool updateExist: false
    property string downloadUrl: info.downloadUrl[os]
    property string webpageUrl: info.webpageUrl
    property string newVersion: info.version
    property int newVersionCode: info.versionCode
    property string message: info.message[lang == "ja_JP" ? 1 : 0]

    property string lastFavoriteData: info.lastFavoriteData
    property string lastTimerSelectGuide: info.lastTimerSelectGuide

    property string version: ""
    property int versionCode: 0
    property int os: 0
    property string lang: ""
    
    // OS number define
    // 0: Windows(32bit)
    // 1: Windows(64bit)
    // 2: Mac OS X(32bit)
    // 3: Mac OS X(64bit)
    // 4: Linux(32bit)
    // 5: Linux(64bit)

    Component.onCompleted: {
        var params
        var items
        
        // Parse User Information
        params = http.query.split('+')
        for(var i=0; i<params.length; i++){
            items = params[i].split('=')
            if(items[0] == 'os'){
                os = 0 + items[1]
                if(os > 5){
                    os = 0
                }
            }else if(items[0] == 'version'){
                version = items[1]
            }else if(items[0] == 'versioncode'){
                versionCode = 0 + items[1]
            }else if(items[0] == 'lang'){
                lang = items[1]
            }else{
            }
        }

        // Check Update exist
        if(newVersionCode > versionCode){
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
                                     , "versionCode": root.versionCode
                                     , "os": root.os
                                     , "lang": root.lang
                                     , "remoteAddress": http.remoteAddress
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

