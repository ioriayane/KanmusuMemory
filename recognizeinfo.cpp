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
#include "recognizeinfo.h"
#include "kanmusumemory_global.h"

#include <QtCore/QRect>
#include <QFile>
#include <QUrl>
#include <QStandardPaths>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDate>

#include <QDebug>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QSettings>

#define RECOGNIZE_INFO_DOWNLOAD_FILE      QString("%1/recognizeinfo.json").arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation))
#define RECOGNIZE_INFO_DOWNLOAD_URL       QUrl("http://relog.xii.jp/download/kancolle/data/recognizeinfo.json")
#define TO_VALUE(array_at, key) QJsonObject(array_at.toObject()).value(key)
#define TO_STRING(array_at, key) QJsonObject(array_at.toObject()).value(key).toString()
#define TO_DOUBLE(array_at, key) QJsonObject(array_at.toObject()).value(key).toDouble()

RecognizeInfo::RecognizeInfo(QObject *parent) :
    QObject(parent)
  , m_currentLoadedDate(2010,1,1)
{

    //遠征のアイテムの四角
    m_itemRectList.append(EXPEDITION_ITEM1_RECT);
    m_itemRectList.append(EXPEDITION_ITEM2_RECT);
    m_itemRectList.append(EXPEDITION_ITEM3_RECT);
    m_itemRectList.append(EXPEDITION_ITEM4_RECT);
    m_itemRectList.append(EXPEDITION_ITEM5_RECT);
    m_itemRectList.append(EXPEDITION_ITEM6_RECT);
    m_itemRectList.append(EXPEDITION_ITEM7_RECT);
    m_itemRectList.append(EXPEDITION_ITEM8_RECT);

    //タイマーの時間のマスク
    m_numberGuideList.append(NumberGuide(":/resources/NumberGuide0.png", 0, 10));
    m_numberGuideList.append(NumberGuide(":/resources/NumberGuide4.png", 4, 14));
    m_numberGuideList.append(NumberGuide(":/resources/NumberGuide1.png", 1, 10));
    m_numberGuideList.append(NumberGuide(":/resources/NumberGuide2.png", 2, 10));
    m_numberGuideList.append(NumberGuide(":/resources/NumberGuide3.png", 3, 10));
    m_numberGuideList.append(NumberGuide(":/resources/NumberGuide5.png", 5, 11));
    m_numberGuideList.append(NumberGuide(":/resources/NumberGuide6.png", 6, 10));
    m_numberGuideList.append(NumberGuide(":/resources/NumberGuide7.png", 7, 14));
    m_numberGuideList.append(NumberGuide(":/resources/NumberGuide9.png", 9, 10));
    m_numberGuideList.append(NumberGuide(":/resources/NumberGuide8.png", 8, 14));

    //残り時間の位置（6個必須）
    m_remainTimeRectList.append(QRect(722, 387, 9, 13));
    m_remainTimeRectList.append(QRect(731, 387, 9, 13));
    m_remainTimeRectList.append(QRect(745, 387, 9, 13));
    m_remainTimeRectList.append(QRect(754, 387, 9, 13));
    m_remainTimeRectList.append(QRect(767, 387, 9, 13));
    m_remainTimeRectList.append(QRect(776, 387, 9, 13));

    //トータル時間の位置（6個必須）
    m_totalTimeRectList.append(QRect(725, 265, 9, 13));
    m_totalTimeRectList.append(QRect(734, 265, 9, 13));
    m_totalTimeRectList.append(QRect(747, 265, 9, 13));
    m_totalTimeRectList.append(QRect(756, 265, 9, 13));
    m_totalTimeRectList.append(QRect(770, 265, 9, 13));
    m_totalTimeRectList.append(QRect(779, 265, 9, 13));


    //遠征の旗のマスク
    m_flagGuideList.append(NumberGuide(":/resources/FlagGuide2.png", 2, 5));
    m_flagGuideList.append(NumberGuide(":/resources/FlagGuide3.png", 3, 5));
    m_flagGuideList.append(NumberGuide(":/resources/FlagGuide4.png", 4, 5));

    //既に遠征に出ているマークの位置
    m_flagRectList.append(QRect(517, 163, 23, 20));
    m_flagRectList.append(QRect(517, 193, 23, 20));
    m_flagRectList.append(QRect(517, 223, 23, 20));
    m_flagRectList.append(QRect(517, 253, 23, 20));
    m_flagRectList.append(QRect(517, 283, 23, 20));
    m_flagRectList.append(QRect(517, 313, 23, 20));
    m_flagRectList.append(QRect(517, 343, 23, 20));
    m_flagRectList.append(QRect(517, 373, 23, 20));


    /////////////////////////////////////////////////////////////
    /// 戦果報告画面
    /////////////////////////////////////////////////////////////
    m_buttleResultRect1 = BUTTLE_RESULT_RECT1;
    m_buttleResultCheckColor1 = BUTTLE_RESULT_CHECK_COLOR1;
    m_buttleResultRect2 = BUTTLE_RESULT_RECT2;
    m_buttleResultCheckColor2 = BUTTLE_RESULT_CHECK_COLOR2;
    m_buttleResultFleetChangeRect = BUTTLE_RESULT_FLEET_CHANGE_RECT;
    //大破判定
    m_buttleResultMajorDamageCheckColor = BUTTLE_RESULT_MAJOR_DAMAGE_CHECK_COLOR;
    m_buttleResultMajorDamageRect = BUTTLE_RESULT_MAJOR_DAMAGE_RECT;
    //進撃or撤退
    m_buttleGoOrBackCheckColor = BUTTLE_GO_OR_BACK_CHECK_COLOR;
    m_buttleGoOrBackRect = BUTTLE_GO_OR_BACK_RECT;
    //羅針盤を回す
    m_buttleCompassCheckColor = BUTTLE_COMPASS_CHECK_COLOR;
    m_buttleCompassRect = BUTTLE_COMPASS_RECT;

    //艦娘バナーの位置
    m_buttleResultCharRectList.append(QRect(196, 189, 160, 40));
    m_buttleResultCharRectList.append(QRect(196, 234, 160, 40));
    m_buttleResultCharRectList.append(QRect(196, 279, 160, 40));
    m_buttleResultCharRectList.append(QRect(196, 324, 160, 40));
    m_buttleResultCharRectList.append(QRect(196, 369, 160, 40));
    m_buttleResultCharRectList.append(QRect(196, 414, 160, 40));
    //艦娘バナーがあるか（艦数判定）
    m_buttleResultCharExistRectList.append(QRect(205, 195, 24, 26));
    m_buttleResultCharExistRectList.append(QRect(205, 240, 24, 26));
    m_buttleResultCharExistRectList.append(QRect(205, 285, 24, 26));
    m_buttleResultCharExistRectList.append(QRect(205, 330, 24, 26));
    m_buttleResultCharExistRectList.append(QRect(205, 375, 24, 26));
    m_buttleResultCharExistRectList.append(QRect(205, 420, 24, 26));
    m_buttleResultCharExistColor.rgb = qRgb(255, 255, 255);
    m_buttleResultCharExistColor.border = 200;
    m_buttleResultCharExistBinBorder = 180;
}



QList<QRect> RecognizeInfo::itemRectList() const
{
    return m_itemRectList;
}

void RecognizeInfo::setItemRectList(const QList<QRect> &itemRectList)
{
    m_itemRectList = itemRectList;
}
QList<NumberGuide> RecognizeInfo::flagGuideList() const
{
    return m_flagGuideList;
}

void RecognizeInfo::setFlagGuideList(const QList<NumberGuide> &flagGuideList)
{
    m_flagGuideList = flagGuideList;
}
QList<QRect> RecognizeInfo::flagRectList() const
{
    return m_flagRectList;
}

void RecognizeInfo::setFlagRectList(const QList<QRect> &flagRectList)
{
    m_flagRectList = flagRectList;
}
QList<NumberGuide> RecognizeInfo::numberGuideList() const
{
    return m_numberGuideList;
}

void RecognizeInfo::setNumberGuideList(const QList<NumberGuide> &numberGuideList)
{
    m_numberGuideList = numberGuideList;
}
QList<QRect> RecognizeInfo::remainTimeRectList() const
{
    return m_remainTimeRectList;
}

void RecognizeInfo::setRemainTimeRectList(const QList<QRect> &remainTimeRectList)
{
    m_remainTimeRectList = remainTimeRectList;
}
QList<QRect> RecognizeInfo::totalTimeRectList() const
{
    return m_totalTimeRectList;
}

void RecognizeInfo::setTotalTimeRectList(const QList<QRect> &totalTimeRectList)
{
    m_totalTimeRectList = totalTimeRectList;
}

//ファイルから読み込む
void RecognizeInfo::load()
{
    QByteArray data;
    QFile file(RECOGNIZE_INFO_DOWNLOAD_FILE);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "not found recognize info";
    }else{
        qDebug() << "found recognize info";

        QTextStream in(&file);
        in.setCodec("UTF-8");
        while(!in.atEnd()){
            data.append(in.readLine());
        }
        file.close();

        QJsonDocument json = QJsonDocument::fromJson(data);
        //遠征のアイテムの四角
        setRectList(&m_itemRectList, json.object().value("itemRect").toArray());
        //タイマーの時間のマスク
        setGuideList(&m_numberGuideList, json.object().value("numberGuide").toArray());
        //残り時間の位置（6個必須）
        setRectList(&m_remainTimeRectList, json.object().value("remainTimeRect").toArray());
        //トータル時間の位置（6個必須）
        setRectList(&m_totalTimeRectList, json.object().value("totalTimeRect").toArray());

        //遠征の旗のマスク  flagGuide
        setGuideList(&m_flagGuideList, json.object().value("flagGuide").toArray());
        //既に遠征に出ているマークの位置
        setRectList(&m_flagRectList, json.object().value("flagRect").toArray());


        //戦果報告画面
        m_buttleResultRect1 = setRect(json.object().value("buttleResultRect1").toObject());
        m_buttleResultCheckColor1 = setColor(json.object().value("buttleResultCheckColor1").toObject());
        m_buttleResultRect2 = setRect(json.object().value("buttleResultRect2").toObject());
        m_buttleResultCheckColor2 = setColor(json.object().value("buttleResultCheckColor2").toObject());
        //戦果報告画面の艦隊切り替わりを判定する因子
        m_buttleResultFleetChangeRect = setRect(json.object().value("buttleResultFleetChangeRect").toObject());
        //大破判定
        m_buttleResultMajorDamageCheckColor = setColor(json.object().value("buttleResultMajorDamageCheckColor").toObject());
        m_buttleResultMajorDamageRect = setRect(json.object().value("buttleResultMajorDamageRect").toObject());
        //進撃or撤退
        m_buttleGoOrBackCheckColor = setColor(json.object().value("buttleGoOrBackCheckColor").toObject());
        m_buttleGoOrBackRect = setRect(json.object().value("buttleGoOrBackRect").toObject());
        //羅針盤を回す
        m_buttleCompassCheckColor = setColor(json.object().value("buttleCompassCheckColor").toObject());
        m_buttleCompassRect = setRect(json.object().value("buttleCompassRect").toObject());

        //艦娘バナーの位置
        setRectList(&m_buttleResultCharRectList, json.object().value("buttleResultCharRectList").toArray());
        //艦娘バナーがあるか（艦数判定）
        setRectList(&m_buttleResultCharExistRectList, json.object().value("buttleResultCharExistRectList").toArray());
        m_buttleResultCharExistColor.rgb = setColor(json.object().value("buttleResultCharExistColor").toObject());
        m_buttleResultCharExistColor.border = setColorBorder(json.object().value("buttleResultCharExistColor").toObject());
        m_buttleResultCharExistBinBorder = json.object().value("buttleResultCharExistBinBorder").toInt();

//        qDebug() << "m_buttleCompassCheckColor " << qRed(m_buttleCompassCheckColor) << "," << qGreen(m_buttleCompassCheckColor) << "," << qBlue(m_buttleCompassCheckColor);
//        qDebug() << "m_buttleCompassRect " << m_buttleCompassRect;

        //現状のデータの日付を保存
        m_currentLoadedDate = QDate::fromString(json.object().value("serial").toString().left(8), "yyyyMMdd");
//        qDebug() << json.object().value("serial").toString().left(8);
//        qDebug() << "serial=" << serial << "," << serial.toJulianDay();
//        qDebug() << "today =" << QDate::currentDate() << "," << QDate::currentDate().toJulianDay();

    }
}

QRgb RecognizeInfo::setColor(const QJsonObject &object)
{
    return qRgba(object.value("red").toInt(), object.value("green").toInt(), object.value("blue").toInt()
                 , object.value("alpha").toInt());
}

int RecognizeInfo::setColorBorder(const QJsonObject &object)
{
    return object.value("border").toInt();
}

QRect RecognizeInfo::setRect(const QJsonObject &object)
{
    return QRect(object.value("x").toDouble(), object.value("y").toDouble()
                 , object.value("width").toDouble(), object.value("height").toDouble());
}

void RecognizeInfo::setRectList(QList<QRect> *list, const QJsonArray &array)
{
    list->clear();
    for(int i=0; i<array.count(); i++){
        //アイテム
//        qDebug() << "x, y, w, h = " << TO_DOUBLE(array.at(i), "x") << "," << TO_DOUBLE(array.at(i), "y")
//                    << "," << TO_DOUBLE(array.at(i), "width") << "," << TO_DOUBLE(array.at(i), "height");
        list->append(QRect(TO_DOUBLE(array.at(i), "x"), TO_DOUBLE(array.at(i), "y")
                          , TO_DOUBLE(array.at(i), "width"), TO_DOUBLE(array.at(i), "height")));
    }
}

void RecognizeInfo::setGuideList(QList<NumberGuide> *list, const QJsonArray &array)
{
    list->clear();
    for(int i=0; i<array.count(); i++){
        //アイテム
        QString path = TO_STRING(array.at(i), "path");
        if(path.left(1) == ":"){
            //リソースパス
        }else{
            //相対設定
            QString temp = QString("%1/numberGuide/").arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
            QDir dir(temp);
            if(!dir.exists()){
                dir.mkpath(temp);
            }
            path = temp + path;
        }
//        qDebug() << "path, number, border = " << path << "," << TO_DOUBLE(array.at(i), "number")
//                    << "," << TO_DOUBLE(array.at(i), "border");
        list->append(NumberGuide(path, TO_DOUBLE(array.at(i), "number"), TO_DOUBLE(array.at(i), "border")));
    }
}

void RecognizeInfo::updateFromInternet(const QString &lastUpdateDate)
{
    if(m_currentLoadedDate.isValid()){
        //ダウンロードデータがあるのでアップデートするかをチェック
        qDebug() << "update recognize " << m_currentLoadedDate << "," << lastUpdateDate;
        if(m_currentLoadedDate < QDate::fromString(lastUpdateDate.left(8), "yyyyMMdd")){
            //ダウンロードする
        }else{
            //ダウンロードしない
            return;
        }
    }

    qDebug() << "start download recognize";
    QNetworkAccessManager *net = new QNetworkAccessManager(this);
    connect(net, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
        if(reply->error() == QNetworkReply::NoError){
            QFile file(RECOGNIZE_INFO_DOWNLOAD_FILE);
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
    //リクエスト作成
    QNetworkRequest req;
    req.setUrl(RECOGNIZE_INFO_DOWNLOAD_URL);
    req.setRawHeader("User-Agent", QString("KanmusuMemory %1").arg(KANMEMO_VERSION).toLatin1());
    //アクセス開始
    net->get(req);

}
int RecognizeInfo::buttleResultCharExistBinBorder() const
{
    return m_buttleResultCharExistBinBorder;
}

void RecognizeInfo::setButtleResultCharExistBinBorder(int buttleResultCharExistBinBorder)
{
    m_buttleResultCharExistBinBorder = buttleResultCharExistBinBorder;
}

QRgbEx RecognizeInfo::buttleResultCharExistColor() const
{
    return m_buttleResultCharExistColor;
}

void RecognizeInfo::setButtleResultCharExistColor(const QRgbEx &buttleResultCharExistColor)
{
    m_buttleResultCharExistColor = buttleResultCharExistColor;
}

QList<QRect> RecognizeInfo::buttleResultCharRectList() const
{
    return m_buttleResultCharRectList;
}

void RecognizeInfo::setButtleResultCharRectList(const QList<QRect> &buttleResultCharRectList)
{
    m_buttleResultCharRectList = buttleResultCharRectList;
}

QRect RecognizeInfo::buttleCompassRect() const
{
    return m_buttleCompassRect;
}

void RecognizeInfo::setButtleCompassRect(const QRect &buttleCompassRect)
{
    m_buttleCompassRect = buttleCompassRect;
}

QRgb RecognizeInfo::buttleCompassCheckColor() const
{
    return m_buttleCompassCheckColor;
}

void RecognizeInfo::setButtleCompassCheckColor(const QRgb &buttleCompassCheckColor)
{
    m_buttleCompassCheckColor = buttleCompassCheckColor;
}



QRect RecognizeInfo::buttleGoOrBackRect() const
{
    return m_buttleGoOrBackRect;
}

void RecognizeInfo::setButtleGoOrBackRect(const QRect &buttleGoOrBackRect)
{
    m_buttleGoOrBackRect = buttleGoOrBackRect;
}

QRgb RecognizeInfo::buttleGoOrBackCheckColor() const
{
    return m_buttleGoOrBackCheckColor;
}

void RecognizeInfo::setButtleGoOrBackCheckColor(const QRgb &buttleGoOrBackCheckColor)
{
    m_buttleGoOrBackCheckColor = buttleGoOrBackCheckColor;
}

QRect RecognizeInfo::buttleResultMajorDamageRect() const
{
    return m_buttleResultMajorDamageRect;
}

void RecognizeInfo::setButtleResultMajorDamageRect(const QRect &buttleResultMajorDamageRect)
{
    m_buttleResultMajorDamageRect = buttleResultMajorDamageRect;
}

QRgb RecognizeInfo::buttleResultMajorDamageCheckColor() const
{
    return m_buttleResultMajorDamageCheckColor;
}

void RecognizeInfo::setButtleResultMajorDamageCheckColor(const QRgb &buttleResultMajorDamageCheckColor)
{
    m_buttleResultMajorDamageCheckColor = buttleResultMajorDamageCheckColor;
}

QRgb RecognizeInfo::buttleResultCheckColor2() const
{
    return m_buttleResultCheckColor2;
}

void RecognizeInfo::setButtleResultCheckColor2(const QRgb &buttleResultCheckColor2)
{
    m_buttleResultCheckColor2 = buttleResultCheckColor2;
}

QRect RecognizeInfo::buttleResultRect2() const
{
    return m_buttleResultRect2;
}

void RecognizeInfo::setButtleResultRect2(const QRect &buttleResultRect2)
{
    m_buttleResultRect2 = buttleResultRect2;
}

QRgb RecognizeInfo::buttleResultCheckColor1() const
{
    return m_buttleResultCheckColor1;
}

void RecognizeInfo::setButtleResultCheckColor1(const QRgb &buttleResultCheckColor1)
{
    m_buttleResultCheckColor1 = buttleResultCheckColor1;
}

QRect RecognizeInfo::buttleResultRect1() const
{
    return m_buttleResultRect1;
}

void RecognizeInfo::setButtleResultRect1(const QRect &buttleResultRect1)
{
    m_buttleResultRect1 = buttleResultRect1;
}

QRect RecognizeInfo::buttleResultFleetChangeRect() const
{
    return m_buttleResultFleetChangeRect;
}

void RecognizeInfo::setButtleResultFleetChangeRect(const QRect &buttleResultFleetChangeRect)
{
    m_buttleResultFleetChangeRect = buttleResultFleetChangeRect;
}

