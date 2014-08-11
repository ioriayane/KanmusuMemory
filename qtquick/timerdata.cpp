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
#include "timerdata.h"

#include <QDebug>

TimerData::TimerData(QObject *parent) :
    QObject(parent)
{
    m_dockingTime << 0 << 0 << 0 << 0;
    m_dockingStart << 0 << 0 << 0 << 0;
    m_dockingRunning << 0 << 0 << 0 << 0;

    m_expeditionTime << 0 << 0 << 0;
    m_expeditionStart << 0 << 0 << 0;
    m_expeditionRunning << 0 << 0 << 0;

    m_constructionTime << 0 << 0 << 0 << 0;
    m_constructionStart << 0 << 0 << 0 << 0;
    m_constructionRunning << 0 << 0 << 0 << 0;

    m_tweetFinished = false;
    m_alarmMute = false;
}

void TimerData::setTime(const int &kind, const int &index, const qreal &time)
{
//    qDebug() << "setTime:" << kind << "," << index << "," << time;
    switch(kind){
    case 0:
        //入渠
        if(m_dockingTime[index] == time)
            break;
        m_dockingTime[index] = time;
        emit dockingTimeChanged(m_dockingTime);

        break;
    case 1:
        //遠征
        if(m_expeditionTime[index] == time)
            break;
        m_expeditionTime[index] = time;
        emit expeditionTimeChanged(m_expeditionTime);

        break;
    case 2:
        //建造
        if(m_constructionTime[index] == time)
            break;
        m_constructionTime[index] = time;
        emit constructionTimeChanged(m_constructionTime);

        break;
    default:
        break;
    }
}

void TimerData::setStartTime(const int &kind, const int &index, const qreal &starttime)
{
//    qDebug() << "setStartTime:" << kind << "," << index << "," << (long long)starttime;
    switch(kind){
    case 0:
        //入渠
        if(m_dockingStart[index] == starttime)
            break;
        m_dockingStart[index] = starttime;
        emit dockingStartChanged(m_dockingStart);

        break;
    case 1:
        //遠征
        if(m_expeditionStart[index] == starttime)
            break;
        m_expeditionStart[index] = starttime;
        emit expeditionStartChanged(m_expeditionStart);

        break;
    case 2:
        //建造
        if(m_constructionStart[index] == starttime)
            break;
        m_constructionStart[index] = starttime;
        emit constructionStartChanged(m_constructionStart);

        break;
    default:
        break;
    }
}

void TimerData::setRunning(const int &kind, const int &index, const bool &running)
{
//    qDebug() << "setRunning:" << kind << "," << index << "," << running;
    switch(kind){
    case 0:
        //入渠
        if(m_dockingRunning[index] == running)
            break;
        m_dockingRunning[index] = running;
        emit dockingRunningChanged(m_dockingRunning);

        break;
    case 1:
        //遠征
        if(m_expeditionRunning[index] == running)
            break;
        m_expeditionRunning[index] = running;
        emit expeditionRunningChanged(m_expeditionRunning);

        break;
    case 2:
        //建造
        if(m_constructionRunning[index] == running)
            break;
        m_constructionRunning[index] = running;
        emit constructionRunningChanged(m_constructionRunning);

        break;
    default:
        break;
    }
}

const QList<qreal> &TimerData::dockingTime() const
{
    return m_dockingTime;
}

void TimerData::setDockingTime(const QList<qreal> &time)
{
    if(m_dockingTime == time)
        return;

    m_dockingTime = time;
    emit dockingTimeChanged(m_dockingTime);
}

const QList<qreal> &TimerData::dockingStart() const
{
    return m_dockingStart;
}

void TimerData::setDockingStart(const QList<qreal> &start)
{
    if(m_dockingStart == start)
        return;

    m_dockingStart = start;
    emit dockingStartChanged(m_dockingStart);
}

const QList<bool> &TimerData::dockingRunning() const
{
    return m_dockingRunning;
}

void TimerData::setDockingRunning(const QList<bool> &running)
{
    if(m_dockingRunning == running)
        return;

    m_dockingRunning = running;
    emit dockingRunningChanged(m_dockingRunning);
}

const QList<qreal> &TimerData::expeditionTime() const
{
    return m_expeditionTime;
}

void TimerData::setExpeditionTime(const QList<qreal> &time)
{
    if(m_expeditionTime == time)
        return;

    m_expeditionTime = time;
    emit expeditionTimeChanged(m_expeditionTime);
}

const QList<qreal> &TimerData::expeditionStart() const
{
    return m_expeditionStart;
}

void TimerData::setExpeditionStart(const QList<qreal> &start)
{
    if(m_expeditionStart == start)
        return;

    m_expeditionStart = start;
}

const QList<bool> &TimerData::expeditionRunning() const
{
    return m_expeditionRunning;
}

void TimerData::setExpeditionRunning(const QList<bool> &running)
{
    if(m_expeditionRunning == running)
        return;

    m_expeditionRunning = running;
    emit expeditionRunningChanged(m_expeditionRunning);
}

const QList<qreal> &TimerData::constructionTime() const
{
    return m_constructionTime;
}

void TimerData::setConstructionTime(const QList<qreal> &time)
{
    if(m_constructionTime == time)
        return;

    m_constructionTime = time;
    emit constructionTimeChanged(m_constructionTime);
}

const QList<qreal> &TimerData::constructionStart() const
{
    return m_constructionStart;
}

void TimerData::setConstructionStart(const QList<qreal> &start)
{
    if(m_constructionStart == start)
        return;

    m_constructionStart = start;
    emit constructionStartChanged(m_constructionStart);
}

const QList<bool> &TimerData::constructionRunning() const
{
    return m_constructionRunning;
}

void TimerData::setConstructionRunning(const QList<bool> &running)
{
    if(m_constructionRunning == running)
        return;

    m_constructionRunning = running;
    emit constructionRunningChanged(m_constructionRunning);
}

//つぶやくか
const bool &TimerData::tweetFinished() const
{
    return m_tweetFinished;
}

void TimerData::setTweetFinished(const bool &tweet)
{
    if(m_tweetFinished == tweet)
        return;

    m_tweetFinished = tweet;
    emit tweetFinishedChanged();
}

const QString &TimerData::alarmSoundPath() const
{
    return m_alarmSoundPath;
}

void TimerData::setAlarmSoundPath(const QString &path)
{
    if(m_alarmSoundPath.compare(path) == 0)
        return;

    m_alarmSoundPath = path;
    emit alarmSoundPathChanged();
}

const qreal &TimerData::alarmSoundVolume() const
{
    return m_alarmSoundVolume;
}
void TimerData::setAlarmSoundVolume(const qreal &volume)
{
    if(m_alarmSoundVolume == volume)
        return;

    m_alarmSoundVolume = volume;
    emit alarmSoundVolumeChanged();
}

QString TimerData::lastUpdateDate() const
{
    return m_lastUpdateDate;
}
void TimerData::setLastUpdateDate(const QString &lastUpdateDate)
{
    if(m_lastUpdateDate == lastUpdateDate)   return;
    m_lastUpdateDate = lastUpdateDate;
    emit lastUpdateDateChanged();
}

bool TimerData::alarmMute() const
{
    return m_alarmMute;
}
void TimerData::setAlarmMute(bool alarmMute)
{
    if(m_alarmMute == alarmMute)    return;
    m_alarmMute = alarmMute;
    emit alarmMuteChanged();
}


bool TimerData::dockingClose() const
{
    return m_dockingClose;
}

void TimerData::setDockingClose(bool dockingClose)
{
    if(m_dockingClose == dockingClose)  return;
    m_dockingClose = dockingClose;
    emit dockingCloseChanged();
}
bool TimerData::expeditionClose() const
{
    return m_expeditionClose;
}

void TimerData::setExpeditionClose(bool expeditionClose)
{
    if(m_expeditionClose == expeditionClose)    return;
    m_expeditionClose = expeditionClose;
    emit expeditionCloseChanged();
}
bool TimerData::constructionClose() const
{
    return m_constructionClose;
}

void TimerData::setConstructionClose(bool constructionClose)
{
    if(m_constructionClose == constructionClose)    return;
    m_constructionClose = constructionClose;
    emit constructionCloseChanged();
}





QList<qreal> TimerData::toRealList(const QList<QVariant> src)
{
    QList<qreal> d;
    foreach (QVariant v, src){
        d << v.toReal();
    }
    return d;
}
QList<bool> TimerData::toBoolList(const QList<QVariant> src)
{
    QList<bool> d;
    foreach (QVariant v, src){
        d << v.toBool();
    }
    return d;
}

QList<int> TimerData::toIntList(const QList<QVariant> src)
{
    QList<int> d;
    foreach (QVariant v, src){
        d << v.toInt();
    }
    return d;
}


