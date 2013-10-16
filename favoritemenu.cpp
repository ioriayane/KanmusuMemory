#include "favoritemenu.h"
#include "kanmusumemory_global.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>
#include <QUrl>
#include <QSettings>
#include <QDebug>

#define TO_VALUE(array_at, key) QJsonObject(array_at.toObject()).value(key)
#define TO_STRING(array_at, key) QJsonObject(array_at.toObject()).value(key).toString()
#define TO_ARRAY(array_at) QJsonObject(array_at.toObject()).value("array").toArray()
#define KEY_TITLE   "title"
#define KEY_URL     "url"

FavoriteMenu::FavoriteMenu(QObject *parent) :
    QObject(parent)
{
}

void FavoriteMenu::load(QMenu *menu)
{
    QByteArray data;
    QFile file(QStringLiteral("favoritedata.json"));
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
    }else{
        QTextStream in(&file);
        in.setCodec("UTF-8");
        while(!in.atEnd()){
            data.append(in.readLine());
        }
        file.close();

        //一旦クリア
        menu->clear();

        QJsonDocument json = QJsonDocument::fromJson(data);
        QJsonArray array = json.object().value("root").toArray();
        QAction *action;
        //フォルダ
        for(int i=0; i<array.count(); i++){
            //アイテム
            if(TO_VALUE(array.at(i), "array").isArray()){
                addItem(menu, &TO_ARRAY(array.at(i)));
            }else{
//            qDebug() << "title:" << TO_STRING(array->at(i), KEY_TITLE);
                action = menu->addAction(TO_STRING(array.at(i), KEY_TITLE), this, SLOT(clickItem()));
                action->setData(TO_STRING(array.at(i), KEY_URL));
            }
        }

        //ユーザー登録ぶん
        QSettings settings(FAV_FILE_NAME, FAV_FILE_FORMAT);
        settings.beginGroup(QStringLiteral(FAV_USER));
        QHash<QString, QVariant> list = settings.value(QStringLiteral(FAV_USER_BOOKMARK)).toHash();
        foreach (const QString &key, list.keys()) {
            action = menu->addAction(list.value(key).toString(), this, SLOT(clickItem()));
            action->setData(key);
        }
        settings.endGroup();
    }
}

void FavoriteMenu::clickItem()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(action){
        if(action->data().toUrl().isValid()){
            qDebug() << "clickItem:" << action->data().toUrl();
            emit selectFav(action->data().toUrl());
        }
    }
}

bool FavoriteMenu::addItem(QMenu *parent, QJsonArray *array)
{
    QMenu *me;
    QAction *action;

    if(array->count() < 2)
        return false;

    //フォルダ
//    qDebug() << "folder:" << TO_STRING(array->at(0), KEY_TITLE) << " - " << array->count();
    me = parent->addMenu(TO_STRING(array->at(0), KEY_TITLE));
    for(int i=1; i<array->count(); i++){
        //アイテム
        if(TO_VALUE(array->at(i), "array").isArray()){
            addItem(me, &TO_ARRAY(array->at(i)));
        }else{
//            qDebug() << "title:" << TO_STRING(array->at(i), KEY_TITLE);
            action = me->addAction(TO_STRING(array->at(i), KEY_TITLE), this, SLOT(clickItem()));
            action->setData(TO_STRING(array->at(i), KEY_URL));
        }
    }

    return true;
}
