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

#include <QObject>
#include "numberguide.h"

//設定をダウンロードしてくる機能
//DLデータはQsettingsつかう？JSON？

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

signals:

public slots:


private:
    QList<QRect> m_itemRectList;            //遠征の項目のエリア

    QList<NumberGuide> m_flagGuideList;     //艦隊番号の旗
    QList<QRect> m_flagRectList;            //遠征の項目につく旗の位置

    QList<NumberGuide> m_numberGuideList;   //数字のガイド
    QList<QRect> m_remainTimeRectList;      //遠征の残り時間
    QList<QRect> m_totalTimeRectList;       //遠征のトータル時間

};

#endif // RECOGNIZEINFO_H
