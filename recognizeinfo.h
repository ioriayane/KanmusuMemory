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
#ifndef RECOGNIZEINFO_H
#define RECOGNIZEINFO_H

#include <QDate>
#include <QObject>
#include "numberguide.h"

//設定をダウンロードしてくる機能
//DLデータはQsettingsつかう？JSON？

class QRgbEx
{
public:
    QRgbEx(){
        rgb = qRgb(255,255,255);
        border = 0x20;
    }
    QRgbEx(QRgb rgb, int border){
        this->rgb = rgb;
        this->border = border;
    }

    QRgb rgb;
    int border;
};

class RecognizeInfo : public QObject
{
    Q_OBJECT
public:
    explicit RecognizeInfo(QObject *parent = 0);

    QList<QRect> itemRectList() const;
    void setItemRectList(const QList<QRect> &itemRectList);
    QList<NumberGuide> flagGuideList() const;
    void setFlagGuideList(const QList<NumberGuide> &flagGuideList);
    QList<QRect> flagRectList() const;
    void setFlagRectList(const QList<QRect> &flagRectList);
    QList<NumberGuide> numberGuideList() const;
    void setNumberGuideList(const QList<NumberGuide> &numberGuideList);
    QList<QRect> remainTimeRectList() const;
    void setRemainTimeRectList(const QList<QRect> &remainTimeRectList);
    QList<QRect> totalTimeRectList() const;
    void setTotalTimeRectList(const QList<QRect> &totalTimeRectList);

    void load();
    QRgb setColor(const QJsonObject &object);
    int setColorBorder(const QJsonObject &object);
    QRect setRect(const QJsonObject &object);
    void setRectList(QList<QRect> *list, const QJsonArray &array);
    void setGuideList(QList<NumberGuide> *list, const QJsonArray &array);

    QRect buttleResultRect1() const;
    void setButtleResultRect1(const QRect &buttleResultRect1);
    QRgb buttleResultCheckColor1() const;
    void setButtleResultCheckColor1(const QRgb &buttleResultCheckColor1);
    QRect buttleResultRect2() const;
    void setButtleResultRect2(const QRect &buttleResultRect2);
    QRgb buttleResultCheckColor2() const;
    void setButtleResultCheckColor2(const QRgb &buttleResultCheckColor2);
    QRect buttleResultFleetChangeRect() const;
    void setButtleResultFleetChangeRect(const QRect &buttleResultFleetChangeRect);

    QRgb buttleResultMajorDamageCheckColor() const;
    void setButtleResultMajorDamageCheckColor(const QRgb &buttleResultMajorDamageCheckColor);
    QRect buttleResultMajorDamageRect() const;
    void setButtleResultMajorDamageRect(const QRect &buttleResultMajorDamageRect);

    QRgb buttleGoOrBackCheckColor() const;
    void setButtleGoOrBackCheckColor(const QRgb &buttleGoOrBackCheckColor);
    QRect buttleGoOrBackRect() const;
    void setButtleGoOrBackRect(const QRect &buttleGoOrBackRect);


    QRgb buttleCompassCheckColor() const;
    void setButtleCompassCheckColor(const QRgb &buttleCompassCheckColor);
    QRect buttleCompassRect() const;
    void setButtleCompassRect(const QRect &buttleCompassRect);

    QList<QRect> buttleResultCharRectList() const;
    void setButtleResultCharRectList(const QList<QRect> &buttleResultCharRectList);

    QRgbEx buttleResultCharExistColor() const;
    void setButtleResultCharExistColor(const QRgbEx &buttleResultCharExistColor);

    int buttleResultCharExistBinBorder() const;
    void setButtleResultCharExistBinBorder(int buttleResultCharExistBinBorder);

signals:
    void downloadFinished();

public slots:
    void updateFromInternet(const QString &lastUpdateDate);


private:
    QList<QRect> m_itemRectList;            //遠征の項目のエリア

    QList<NumberGuide> m_flagGuideList;     //艦隊番号の旗
    QList<QRect> m_flagRectList;            //遠征の項目につく旗の位置

    QList<NumberGuide> m_numberGuideList;   //数字のガイド
    QList<QRect> m_remainTimeRectList;      //遠征の残り時間
    QList<QRect> m_totalTimeRectList;       //遠征のトータル時間

    //戦果報告画面
    QRect m_buttleResultRect1;              //戦果画面画面判定1
    QRgb m_buttleResultCheckColor1;         //戦果報告画面判定1
    QRect m_buttleResultRect2;              //戦果画面画面判定2
    QRgb m_buttleResultCheckColor2;         //戦果報告画面判定2
    QRect m_buttleResultFleetChangeRect;    //戦果報告画面の艦隊切り替わりを判定する因子
    //大破判定
    QRgb m_buttleResultMajorDamageCheckColor;
    QRect m_buttleResultMajorDamageRect;
    //進撃or撤退
    QRgb m_buttleGoOrBackCheckColor;
    QRect m_buttleGoOrBackRect;
    //羅針盤を回す
    QRgb m_buttleCompassCheckColor;
    QRect m_buttleCompassRect;
    //艦娘バナーの位置
    QList<QRect> m_buttleResultCharRectList;
    //艦娘バナーがあるか（艦数判定）
    QList<QRect> m_buttleResultCharExistRectList;   //バナーごとの位置
    QRgbEx m_buttleResultCharExistColor;            //色判定用
    int m_buttleResultCharExistBinBorder;           //2値化するときのしきい値

    QDate m_currentLoadedDate;              //現在読み込んでるデータの作成日

};

#endif // RECOGNIZEINFO_H
