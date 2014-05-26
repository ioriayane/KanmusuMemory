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


RecognizeInfo::RecognizeInfo(QObject *parent) :
    QObject(parent)
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

    //タイマーの時間の位置
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


    //遠征の旗の位置
    m_flagGuideList.append(NumberGuide(":/resources/FlagGuide2.png", 2, 5));
    m_flagGuideList.append(NumberGuide(":/resources/FlagGuide3.png", 3, 5));
    m_flagGuideList.append(NumberGuide(":/resources/FlagGuide4.png", 4, 5));

    m_flagRectList.append(QRect(517, 163, 23, 20));
    m_flagRectList.append(QRect(517, 193, 23, 20));
    m_flagRectList.append(QRect(517, 223, 23, 20));
    m_flagRectList.append(QRect(517, 253, 23, 20));
    m_flagRectList.append(QRect(517, 283, 23, 20));
    m_flagRectList.append(QRect(517, 313, 23, 20));
    m_flagRectList.append(QRect(517, 343, 23, 20));
    m_flagRectList.append(QRect(517, 373, 23, 20));
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






