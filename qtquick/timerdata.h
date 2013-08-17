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

//    property variant dockingSet: [0, 7830000, 0, 0]
//    property variant dockingStart: [0, 0, 0, 0]
//    property variant dockingRunning: [false, false, false, false]
//    //遠征
//    property variant expeditionSet: [0, 7830000, 0, 0]
//    property variant expeditionStart: [0, 0, 0, 0]
//    property variant expeditionRunning: [false, false, false, false]
//    //建造
//    property variant constructionSet: [0, 30000, 0, 0]
//    property variant constructionStart: [0, 0, 0, 0]
//    property variant constructionRunning: [false, true, false, false]

    Q_PROPERTY(QList<qreal> dockingTime READ dockingTime WRITE setDockingTime NOTIFY dockingTimeChanged)
    Q_PROPERTY(QList<qreal> dockingStart READ dockingStart WRITE setDockingStart NOTIFY dockingStartChanged)
    Q_PROPERTY(QList<bool> dockingRunning READ dockingRunning WRITE setDockingRunning NOTIFY dockingRunningChanged)

    Q_PROPERTY(QList<qreal> expeditionTime READ expeditionTime WRITE setExpeditionTime NOTIFY expeditionTimeChanged)
    Q_PROPERTY(QList<qreal> expeditionStart READ expeditionStart WRITE setExpeditionStart NOTIFY expeditionStartChanged)
    Q_PROPERTY(QList<bool> expeditionRunning READ expeditionRunning WRITE setExpeditionRunning NOTIFY expeditionRunningChanged)

    Q_PROPERTY(QList<qreal> constructionTime READ constructionTime WRITE setConstructionTime NOTIFY constructionTimeChanged)
    Q_PROPERTY(QList<qreal> constructionStart READ constructionStart WRITE setConstructionStart NOTIFY constructionStartChanged)
    Q_PROPERTY(QList<bool> constructionRunning READ constructionRunning WRITE setConstructionRunning NOTIFY constructionRunningChanged)

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
};

#endif // TIMERDATA_H
