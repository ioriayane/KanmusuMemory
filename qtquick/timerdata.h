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
#ifndef TIMERDATA_H
#define TIMERDATA_H

#include <QObject>
#include <QList>

class TimerData : public QObject
{
    Q_OBJECT

    //入渠
    Q_PROPERTY(QList<qreal> dockingTime READ dockingTime WRITE setDockingTime NOTIFY dockingTimeChanged)
    Q_PROPERTY(QList<qreal> dockingStart READ dockingStart WRITE setDockingStart NOTIFY dockingStartChanged)
    Q_PROPERTY(QList<bool> dockingRunning READ dockingRunning WRITE setDockingRunning NOTIFY dockingRunningChanged)
    //遠征
    Q_PROPERTY(QList<qreal> expeditionTime READ expeditionTime WRITE setExpeditionTime NOTIFY expeditionTimeChanged)
    Q_PROPERTY(QList<qreal> expeditionStart READ expeditionStart WRITE setExpeditionStart NOTIFY expeditionStartChanged)
    Q_PROPERTY(QList<bool> expeditionRunning READ expeditionRunning WRITE setExpeditionRunning NOTIFY expeditionRunningChanged)
    //建造
    Q_PROPERTY(QList<qreal> constructionTime READ constructionTime WRITE setConstructionTime NOTIFY constructionTimeChanged)
    Q_PROPERTY(QList<qreal> constructionStart READ constructionStart WRITE setConstructionStart NOTIFY constructionStartChanged)
    Q_PROPERTY(QList<bool> constructionRunning READ constructionRunning WRITE setConstructionRunning NOTIFY constructionRunningChanged)

    //設定
    Q_PROPERTY(bool tweetFinished READ tweetFinished WRITE setTweetFinished NOTIFY tweetFinishedChanged)
    Q_PROPERTY(QString alarmSoundPath READ alarmSoundPath WRITE setAlarmSoundPath NOTIFY alarmSoundPathChanged)
    Q_PROPERTY(qreal alarmSoundVolume READ alarmSoundVolume WRITE setAlarmSoundVolume NOTIFY alarmSoundVolumeChanged)
    Q_PROPERTY(QString lastUpdateDate READ lastUpdateDate WRITE setLastUpdateDate NOTIFY lastUpdateDateChanged)

    //折りたたみ
    Q_PROPERTY(bool dockingClose READ dockingClose WRITE setDockingClose NOTIFY dockingCloseChanged)
    Q_PROPERTY(bool expeditionClose READ expeditionClose WRITE setExpeditionClose NOTIFY expeditionCloseChanged)
    Q_PROPERTY(bool constructionClose READ constructionClose WRITE setConstructionClose NOTIFY constructionCloseChanged)

public:
    explicit TimerData(QObject *parent = 0);
    
    Q_INVOKABLE void setTime(const int &kind, const int &index, const qreal &time);
    Q_INVOKABLE void setStartTime(const int &kind, const int &index, const qreal &starttime);
    Q_INVOKABLE void setRunning(const int &kind, const int &index, const bool &running);

    const QList<qreal> &dockingTime() const;
    void setDockingTime(const QList<qreal> &time);
    const QList<qreal> &dockingStart() const;
    void setDockingStart(const QList<qreal> &start);
    const QList<bool> &dockingRunning() const;
    void setDockingRunning(const QList<bool> &running);

    const QList<qreal> &expeditionTime() const;
    void setExpeditionTime(const QList<qreal> &time);
    const QList<qreal> &expeditionStart() const;
    void setExpeditionStart(const QList<qreal> &start);
    const QList<bool> &expeditionRunning() const;
    void setExpeditionRunning(const QList<bool> &running);

    const QList<qreal> &constructionTime() const;
    void setConstructionTime(const QList<qreal> &time);
    const QList<qreal> &constructionStart() const;
    void setConstructionStart(const QList<qreal> &start);
    const QList<bool> &constructionRunning() const;
    void setConstructionRunning(const QList<bool> &running);

    const bool &tweetFinished() const;
    void setTweetFinished(const bool &tweet);
    const QString &alarmSoundPath() const;
    void setAlarmSoundPath(const QString &path);
    const qreal &alarmSoundVolume() const;
    void setAlarmSoundVolume(const qreal &volume);

    static QList<qreal> toRealList(const QList<QVariant> src);
    static QList<bool> toBoolList(const QList<QVariant> src);
    static QList<int> toIntList(const QList<QVariant> src);
    template <class UD, class US>
    static QList<UD> toList(const QList<US> &src)
    {
        QList<UD> dest;
        foreach (US s, src){
            dest << s;
        }
        return dest;
    }

    QString lastUpdateDate() const;
    void setLastUpdateDate(const QString &lastUpdateDate);

    //折りたたみ系
    bool dockingClose() const;
    void setDockingClose(bool dockingClose);
    bool expeditionClose() const;
    void setExpeditionClose(bool expeditionClose);
    bool constructionClose() const;
    void setConstructionClose(bool constructionClose);

signals:
    void dockingTimeChanged(const QList<qreal> &set);
    void dockingStartChanged(const QList<qreal> &start);
    void dockingRunningChanged(const QList<bool> &running);

    void expeditionTimeChanged(const QList<qreal> &set);
    void expeditionStartChanged(const QList<qreal> &start);
    void expeditionRunningChanged(const QList<bool> &running);

    void constructionTimeChanged(const QList<qreal> &set);
    void constructionStartChanged(const QList<qreal> &start);
    void constructionRunningChanged(const QList<bool> &running);

    void tweetFinishedChanged();
    void alarmSoundPathChanged();
    void alarmSoundVolumeChanged();
    void lastUpdateDateChanged();

    void dockingCloseChanged();
    void expeditionCloseChanged();
    void constructionCloseChanged();
public slots:

private:
    //入渠
    QList<qreal> m_dockingTime;
    QList<qreal> m_dockingStart;
    QList<bool> m_dockingRunning;
    //遠征
    QList<qreal> m_expeditionTime;
    QList<qreal> m_expeditionStart;
    QList<bool> m_expeditionRunning;
    //建造
    QList<qreal> m_constructionTime;
    QList<qreal> m_constructionStart;
    QList<bool> m_constructionRunning;

    bool m_tweetFinished;       //時間になったらつぶやくか
    QString m_alarmSoundPath;   //アラーム音のパス
    qreal m_alarmSoundVolume;   //アラーム音量

    QString m_lastUpdateDate;   //データの更新日

    //折りたたみ
    bool m_dockingClose;
    bool m_expeditionClose;
    bool m_constructionClose;
};

#endif // TIMERDATA_H
