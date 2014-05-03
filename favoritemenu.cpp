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
#include "favoritemenu.h"
#include "kanmusumemory_global.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QStandardPaths>
#include <QDebug>

#define FAVORITE_DOWNLOAD_FILE      QString("%1/favoritedata.json").arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation))
#define FAVORITE_DOWNLOAD_URL       QUrl("http://relog.xii.jp/download/kancolle/data/favoritedata.json")
#define TO_VALUE(array_at, key) QJsonObject(array_at.toObject()).value(key)
#define TO_STRING(array_at, key) QJsonObject(array_at.toObject()).value(key).toString()
#define TO_ARRAY(array_at) (QJsonObject(array_at.toObject()).value("array").toArray())
#define KEY_TITLE   "title"
#define KEY_URL     "url"

FavoriteMenu::FavoriteMenu(QObject *parent) :
    QObject(parent)
  , m_currentLoadedFavDataDate(2010,1,1)
{
}

void FavoriteMenu::load(QMenu *menu, bool download)
{
    //一旦クリア
    menu->clear();

    QAction *action;
    QByteArray data;
    QFile file(FAVORITE_DOWNLOAD_FILE);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
    }else{
        QTextStream in(&file);
        in.setCodec("UTF-8");
        while(!in.atEnd()){
            data.append(in.readLine());
        }
        file.close();

        QJsonDocument json = QJsonDocument::fromJson(data);
        QJsonArray array = json.object().value("root").toArray();
        //フォルダ
        for(int i=0; i<array.count(); i++){
            //アイテム
            if(TO_VALUE(array.at(i), "array").isArray()){
                addItem(menu, TO_ARRAY(array.at(i)));
            }else{
//            qDebug() << "title:" << TO_STRING(array->at(i), KEY_TITLE);
                action = menu->addAction(TO_STRING(array.at(i), KEY_TITLE), this, SLOT(clickItem()));
                action->setData(TO_STRING(array.at(i), KEY_URL));
            }
        }

        //現状のデータの日付を保存
        m_currentLoadedFavDataDate = QDate::fromString(json.object().value("serial").toString().left(8), "yyyyMMdd");
//        qDebug() << json.object().value("serial").toString().left(8);
//        qDebug() << "serial=" << serial << "," << serial.toJulianDay();
//        qDebug() << "today =" << QDate::currentDate() << "," << QDate::currentDate().toJulianDay();

    }

    //ユーザー登録ぶん
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, KANMEMO_PROJECT, KANMEMO_NAME_FAV);
    settings.beginGroup(QStringLiteral(FAV_USER));
    QHash<QString, QVariant> list = settings.value(QStringLiteral(FAV_USER_BOOKMARK)).toHash();
    foreach (const QString &key, list.keys()) {
        action = menu->addAction(list.value(key).toString(), this, SLOT(clickItem()));
        action->setData(key);
    }
    settings.endGroup();

}

//ダウンロードしてアップデートする
void FavoriteMenu::updateFromInternet(const QString &lastUpdateDate)
{
    if(m_currentLoadedFavDataDate.isValid()){
        //ダウンロードデータがあるのでアップデートするかをチェック
//        qDebug() << "update fav " << m_currentLoadedFavDataDate << "," << lastUpdateDate;
        if(m_currentLoadedFavDataDate < QDate::fromString(lastUpdateDate.left(8), "yyyyMMdd")){
            //ダウンロードする
        }else{
            //ダウンロードしない
            return;
        }
    }

//    qDebug() << "start download";
    QNetworkAccessManager *net = new QNetworkAccessManager(this);
    connect(net, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
        if(reply->error() == QNetworkReply::NoError){
            QFile file(FAVORITE_DOWNLOAD_FILE);
            if(file.open(QIODevice::WriteOnly)){
                file.write(reply->readAll());
                file.close();
                //通知
                emit downloadFinished();
            }
        }
    });
    //プロキシ
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, KANMEMO_PROJECT, KANMEMO_NAME);
    QNetworkProxy *proxy = new QNetworkProxy();
    bool enable = settings.value(SETTING_GENERAL_PROXY_ENABLE, false).toBool();
    QString host = settings.value(SETTING_GENERAL_PROXY_HOST).toString();
    if(host.length() > 0 && enable){
        proxy->setType(QNetworkProxy::HttpProxy);
        proxy->setHostName(host);
        proxy->setPort(settings.value(SETTING_GENERAL_PROXY_PORT, 8888).toInt());
        net->setProxy(*proxy);
    }else{
        net->setProxy(QNetworkProxy::NoProxy);
    }
    net->get(QNetworkRequest(FAVORITE_DOWNLOAD_URL));
}

void FavoriteMenu::clickItem()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(action){
        if(action->data().toUrl().isValid()){
//            qDebug() << "clickItem:" << action->data().toUrl();
            emit selectFav(action->data().toUrl());
        }
    }
}

bool FavoriteMenu::addItem(QMenu *parent, const QJsonArray &array)
{
    QMenu *me;
    QAction *action;

    if(array.count() < 2)
        return false;

    //フォルダ
//    qDebug() << "folder:" << TO_STRING(array->at(0), KEY_TITLE) << " - " << array->count();
    me = parent->addMenu(TO_STRING(array.at(0), KEY_TITLE));
    for(int i=1; i<array.count(); i++){
        //アイテム
        if(TO_VALUE(array.at(i), "array").isArray()){
            addItem(me, TO_ARRAY(array.at(i)));
        }else{
//            qDebug() << "title:" << TO_STRING(array->at(i), KEY_TITLE);
            action = me->addAction(TO_STRING(array.at(i), KEY_TITLE), this, SLOT(clickItem()));
            action->setData(TO_STRING(array.at(i), KEY_URL));
        }
    }

    return true;
}
