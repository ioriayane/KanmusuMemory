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
#include "timerdialog.h"
#include "ui_timerdialog.h"
#include "kanmusumemory_global.h"
#include "../KanmusuMemoryTwitter/twitterinfo.h"

#include <QQmlContext>
#include <QQuickItem>
#include <QtQml/QQmlEngine>
#include <QDateTime>

#include <QtCore/QDebug>

static const int DIALOG_MARGIN = 30;

TimerDialog::TimerDialog(QWidget *parent
                         , QSystemTrayIcon *trayIcon
                         , QSettings *settings) :
    QDialog(parent),
    m_viewer(NULL),
    m_trayIcon(trayIcon),
    m_settings(settings),
    m_oauth(this),
    m_status(this),
    ui(new Ui::TimerDialog)
{
    ui->setupUi(this);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(&m_timerdata, &TimerData::tweetFinishedChanged, [this](){
        if(m_settings != NULL){
            m_settings->beginGroup(QStringLiteral(SETTING_TIMER));
            m_settings->setValue(QStringLiteral(SETTING_TIMER_TWEETFINISHED), m_timerdata.tweetFinished());
            m_settings->endGroup();
        }
    });
    connect(&m_status, &Status::loadingChanged, [this](bool loading) {
        if(loading){
            //送信中
        }else{
            //終わった
            tweetTimerMessage(QStringList());
        }
    });

    loadSettings();

    //QMLの読み込み
    m_viewer = new QtQuick2ApplicationViewer(windowHandle());
    connect(m_viewer->engine(), SIGNAL(quit()), this, SLOT(closeQml()));
    //C++のデータをQML側へ公開
    m_viewer->rootContext()->setContextProperty("timerData", &m_timerdata);
    //QML設定して表示
    m_viewer->setResizeMode(QQuickView::SizeViewToRootObject);
    m_viewer->setSource(QUrl("qrc:///qml/KanmusuMemory/timerDialog.qml"));
    ui->layout->addWidget(QWidget::createWindowContainer(m_viewer, this));
    //サイズ調節
//    QSize contentSize = m_viewer->rootObject()->childrenRect().toRect().size() + QSize(DIALOG_MARGIN,DIALOG_MARGIN);
//    setMinimumSize(contentSize);
//    setMaximumSize(contentSize);
    QSize size = QSize(m_viewer->rootObject()->width(), m_viewer->rootObject()->height());
//    setMinimumSize(size);
    setMaximumSize(size);
    connect(m_viewer->rootObject(), &QQuickItem::widthChanged, [this](){
        resize(m_viewer->rootObject()->width(), height());
    });
    connect(m_viewer->rootObject(), &QQuickItem::heightChanged, [this](){
        resize(m_viewer->rootObject()->width(), m_viewer->rootObject()->height());
    });

    m_timer.start(10000);
}

TimerDialog::~TimerDialog()
{
    //設定保存
    saveSettings();

    if(m_viewer != NULL)
        delete m_viewer;
    delete ui;
}

void TimerDialog::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if(m_viewer != NULL){
        m_viewer->setGeometry(QRect(0, 0, width(), height()));
    }
}

void TimerDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
}


void TimerDialog::closeQml()
{
    //    accept();
}

//全体のカウント処理
void TimerDialog::timeout()
{
    QString message;
    QStringList messages;
    QStringList numbername;
    numbername << tr("1st") << tr("2nd") << tr("3rd") << tr("4th") << tr("5th") << tr("6th") << tr("7th") << tr("8th");

    //入渠
    message = "";
    for(int i=0; i<m_timerdata.dockingRunning().length(); i++){
        if(m_timerdata.dockingRunning()[i]){
            //            qDebug() << i << ":set=" << m_timerdata.dockingTime()[i]
            //                        << ",start=" << m_timerdata.dockingStart()[i];
            if(checkKanmemoTimerTimeout(m_timerdata.dockingTime()[i]
                                        , m_timerdata.dockingStart()[i])){
                //                qDebug() << "doc:" << i << ":" << "timeout";
                m_timerdata.setRunning(0, i, false);

                //メッセージ追加
                if(message.length() == 0){
                    message = tr("It is time that Kanmusu is up from the ");
                }else{
                    message.append(",");
                }
                message.append(numbername[i]);
//                messages.append(tr("It is time that Kanmusu is up from the %1 bath.").arg(numbername[i]));
                //                messages.append(tr("ドック%1の艦娘がお風呂から上がる頃です。").arg(i+1));
            }
        }
    }
    if(message.length() > 0){
        message.append(tr(" bath."));
        messages.append(message);
    }

    //遠征
    message = "";
    for(int i=0; i<m_timerdata.expeditionRunning().length(); i++){
        if(m_timerdata.expeditionRunning()[i]){
            if(checkKanmemoTimerTimeout(m_timerdata.expeditionTime()[i]
                                        , m_timerdata.expeditionStart()[i])){
                //                qDebug() << "exp:" << i << ":" << "timeout";
                m_timerdata.setRunning(1, i, false);

                //メッセージ追加
                if(message.length() == 0){
                    message = tr("It is time the ");
                }else{
                    message.append(",");
                }
                message.append(numbername[i+1]);
//                messages.append(tr("It is time the %1 Fleet is returned.").arg(numbername[i+1]));
            }
        }
    }
    if(message.length() > 0){
        message.append(tr(" Fleet is returned."));
        messages.append(message);
    }

    //建造
    message = "";
    for(int i=0; i<m_timerdata.constructionRunning().length(); i++){
        if(m_timerdata.constructionRunning()[i]){
            if(checkKanmemoTimerTimeout(m_timerdata.constructionTime()[i]
                                        , m_timerdata.constructionStart()[i])){
                //                qDebug() << "con:" << i << ":" << "timeout";
                m_timerdata.setRunning(2, i, false);

                //メッセージ追加
                if(message.length() == 0){
                    message = tr("It is time the Kanmusu of ");
                }else{
                    message.append(",");
                }
                message.append(numbername[i]);
//                messages.append(tr("It is time the Kanmusu of %1 dock is completed.").arg(numbername[i]));
            }
        }
    }
    if(message.length() > 0){
        message.append(tr(" dock is completed."));
        messages.append(message);
    }

    if(messages.length() > 0){
        //システムトレイ
        showTimerMessage(messages);

        //つぶやく
        tweetTimerMessage(messages);
    }

}

QString TimerDialog::lastTimerSelectGuideUpdateDate() const
{
    return m_timerdata.lastUpdateDate();
}

void TimerDialog::setLastTimerSelectGuideUpdateDate(const QString &lastTimerSelectGuideUpdateDate)
{
    m_timerdata.setLastUpdateDate(lastTimerSelectGuideUpdateDate);
}

//タイマーの設定時間を更新する
void TimerDialog::updateTimerSetting(const int kind, const int fleet_no, const qint64 remain, const qint64 total)
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    int index = fleet_no - 2;

    if(index < 0)   index = 0;
    if(index >= 3)  index = 2;

    if(m_viewer != NULL){
        QMetaObject::invokeMethod(m_viewer->rootObject(), "stop"
                                  , Q_ARG(QVariant, QVariant::fromValue(kind))
                                  , Q_ARG(QVariant, QVariant::fromValue(index)));
    }
    m_timerdata.setTime(kind, index, total);
    m_timerdata.setStartTime(kind, index, now - (total - remain));
    m_timerdata.setRunning(kind, index, true);
}

const bool TimerDialog::tweetFinished() const
{
    return m_timerdata.tweetFinished();
}
void TimerDialog::setTweetFinished(bool tweet)
{
    m_timerdata.setTweetFinished(tweet);
}

void TimerDialog::setAlarmMute(bool mute)
{
    m_timerdata.setAlarmMute(mute);
}


//時間が来ているかチェックする
bool TimerDialog::checkKanmemoTimerTimeout(qint64 settime, qint64 starttime)
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    if(now >= (settime + starttime)){
        //時間が来た
        return true;
    }else{
        return false;
    }
}

void TimerDialog::showTimerMessage(const QStringList &messages)
{
    if(m_trayIcon == NULL)
        return;

    if(!m_trayIcon->isVisible())
        return;

    QString msg;
    for(int i=0; i<messages.length(); i++){
        msg.append(messages[i] + "\n");
    }
    m_trayIcon->showMessage(tr("KanMemo"), msg
                            , QSystemTrayIcon::Information, 5000);
}

//つぶやく
void TimerDialog::tweetTimerMessage(const QStringList &messages)
{
    if(!m_timerdata.tweetFinished())
        return;

    if(m_settings == NULL)
        return;

    m_oauth.setConsumerKey(TWITTER_CONSUMER_KEY);
    m_oauth.setConsumerSecret(TWITTER_CONSUMER_SECRET);
    m_oauth.setToken(m_settings->value(SETTING_GENERAL_TOKEN, "").toString());
    m_oauth.setTokenSecret(m_settings->value(SETTING_GENERAL_TOKENSECRET, "").toString());
    m_oauth.user_id(m_settings->value(SETTING_GENERAL_USER_ID, "").toString());
    m_oauth.screen_name(m_settings->value(SETTING_GENERAL_SCREEN_NAME, "").toString());

    int tweet_len_limit = 140;
    tweet_len_limit -= m_oauth.screen_name().length() + 2;
    tweet_len_limit -= tr("#kanmemo").length();
    tweet_len_limit -= 12 + 1 + 1 + 1 + messages.length();

    qDebug() << "tweet len limit=" << tweet_len_limit;

    if(m_oauth.state() == OAuth::Authorized){
        QString message;

        if(messages.length() > 0){
            //キューに登録したいメッセージがあれば登録
            message = "@" + m_oauth.screen_name() + " ";
            for(int i=0; i<messages.length(); i++){
                QString temp = messages[i];

                if((message.length() + temp.length() + 1) >= tweet_len_limit){
                    //文字数オーバー一旦送信
                    message.append(QStringLiteral("%1 %2")
                                   .arg(QDateTime::currentDateTime().toString(QStringLiteral("MM/dd hh:mm")))
                                   .arg(tr("#kanmemo")));
                    //                qDebug() << message.length() << "," << message;
                    //リストに貯めこむ
                    //qDebug() << "tweet1:" << message;
                    m_tweetMessageList.append(message);

                    //消す
                    message = "@" + m_oauth.screen_name() + " ";
                }
                //連結
                message.append(temp + "\n ");
            }
            message.append(QStringLiteral("%1 %2")
                           .arg(QDateTime::currentDateTime().toString(QStringLiteral("MM/dd hh:mm")))
                           .arg(tr("#kanmemo")));
            //        qDebug() << message.length() << "," << message;

            //リストに貯めこむ
            //qDebug() << "tweet2:" << message;
            m_tweetMessageList.append(message);
        }

        if(m_status.loading()){
            qDebug() << "now tweeting (timer dialog)";
        }else if(!m_tweetMessageList.isEmpty()){
            //リストから取り出してつぶやく
            qDebug() << "tweet:" << m_tweetMessageList.first();
            QVariantMap map;
            map.insert("status", m_tweetMessageList.first());
            m_status.statusesUpdate(map);
            m_tweetMessageList.removeFirst();   //消す
        }else{
            qDebug() << "tweet list empty";
        }
    }
}

void TimerDialog::loadSettings()
{
    if(m_settings == NULL)
        return;

    //設定読み込み
    m_settings->beginGroup(QStringLiteral(SETTING_TIMER));
    //入渠
    m_timerdata.setDockingTime(TimerData::toRealList(m_settings->value(QStringLiteral(SETTING_TIMER_DOCKING_TIME), QList<QVariant>() << 0 << 0 << 0 << 0).toList()));
    m_timerdata.setDockingStart(TimerData::toRealList(m_settings->value(QStringLiteral(SETTING_TIMER_DOCKING_START), QList<QVariant>() << 0 << 0 << 0 << 0).toList()));
    m_timerdata.setDockingRunning(TimerData::toBoolList(m_settings->value(QStringLiteral(SETTING_TIMER_DOCKING_RUNNING), QList<QVariant>() << 0 << 0 << 0 << 0).toList()));
    //遠征
    m_timerdata.setExpeditionTime(TimerData::toRealList(m_settings->value(QStringLiteral(SETTING_TIMER_EXPEDITION_TIME), QList<QVariant>() << 0 << 0 << 0).toList()));
    m_timerdata.setExpeditionStart(TimerData::toRealList(m_settings->value(QStringLiteral(SETTING_TIMER_EXPEDITION_START), QList<QVariant>() << 0 << 0 << 0).toList()));
    m_timerdata.setExpeditionRunning(TimerData::toBoolList(m_settings->value(QStringLiteral(SETTING_TIMER_EXPEDITION_RUNNING), QList<QVariant>() << 0 << 0 << 0).toList()));
    //建造
    m_timerdata.setConstructionTime(TimerData::toRealList(m_settings->value(QStringLiteral(SETTING_TIMER_CONSTRUCTION_TIME), QList<QVariant>() << 0 << 0 << 0 << 0).toList()));
    m_timerdata.setConstructionStart(TimerData::toRealList(m_settings->value(QStringLiteral(SETTING_TIMER_CONSTRUCTION_START), QList<QVariant>() << 0 << 0 << 0 << 0).toList()));
    m_timerdata.setConstructionRunning(TimerData::toBoolList(m_settings->value(QStringLiteral(SETTING_TIMER_CONSTRUCTION_RUNNING), QList<QVariant>() << 0 << 0 << 0 << 0).toList()));

    //つぶやくか
    m_timerdata.setTweetFinished(m_settings->value(QStringLiteral(SETTING_TIMER_TWEETFINISHED), true).toBool());
    //アラームの設定（暫定で固定）
#ifdef Q_OS_MAC
    m_timerdata.setAlarmSoundPath(QString("%1/../../../alarm.mp3")
                                  .arg(QCoreApplication::applicationDirPath()));
#else
    m_timerdata.setAlarmSoundPath(QString("%1/alarm.mp3")
                                  .arg(QCoreApplication::applicationDirPath()));
#endif
    m_timerdata.setAlarmSoundVolume(0.4);

    //折りたたみ
    m_timerdata.setDockingClose(m_settings->value(QStringLiteral(SETTING_TIMER_DOCKING_CLOSE), false).toBool());
    m_timerdata.setExpeditionClose(m_settings->value(QStringLiteral(SETTING_TIMER_EXPEDITION_CLOSE), false).toBool());
    m_timerdata.setConstructionClose(m_settings->value(QStringLiteral(SETTING_TIMER_CONSTRUCTION_CLOSE), false).toBool());
    m_settings->endGroup();


    //ウインドウの位置を復元
    m_settings->beginGroup(QStringLiteral(SETTING_TIMERDIALOG));
    restoreGeometry(m_settings->value(QStringLiteral(SETTING_WINDOW_GEO)).toByteArray());
    m_settings->endGroup();
}

void TimerDialog::saveSettings()
{
    if(m_settings == NULL)
        return;

    //保存
    m_settings->beginGroup(QStringLiteral(SETTING_TIMER));
    //入渠
    m_settings->setValue(QStringLiteral(SETTING_TIMER_DOCKING_TIME), TimerData::toList<QVariant, qreal>(m_timerdata.dockingTime()));
    m_settings->setValue(QStringLiteral(SETTING_TIMER_DOCKING_START), TimerData::toList<QVariant, qreal>(m_timerdata.dockingStart()));
    m_settings->setValue(QStringLiteral(SETTING_TIMER_DOCKING_RUNNING), TimerData::toList<QVariant, bool>(m_timerdata.dockingRunning()));
    //遠征
    m_settings->setValue(QStringLiteral(SETTING_TIMER_EXPEDITION_TIME), TimerData::toList<QVariant, qreal>(m_timerdata.expeditionTime()));
    m_settings->setValue(QStringLiteral(SETTING_TIMER_EXPEDITION_START), TimerData::toList<QVariant, qreal>(m_timerdata.expeditionStart()));
    m_settings->setValue(QStringLiteral(SETTING_TIMER_EXPEDITION_RUNNING), TimerData::toList<QVariant, bool>(m_timerdata.expeditionRunning()));
    //建造
    m_settings->setValue(QStringLiteral(SETTING_TIMER_CONSTRUCTION_TIME), TimerData::toList<QVariant, qreal>(m_timerdata.constructionTime()));
    m_settings->setValue(QStringLiteral(SETTING_TIMER_CONSTRUCTION_START), TimerData::toList<QVariant, qreal>(m_timerdata.constructionStart()));
    m_settings->setValue(QStringLiteral(SETTING_TIMER_CONSTRUCTION_RUNNING), TimerData::toList<QVariant, bool>(m_timerdata.constructionRunning()));
    //つぶやくか
    m_settings->setValue(QStringLiteral(SETTING_TIMER_TWEETFINISHED), m_timerdata.tweetFinished());
    //折りたたみ
    m_settings->setValue(QStringLiteral(SETTING_TIMER_DOCKING_CLOSE), m_timerdata.dockingClose());
    m_settings->setValue(QStringLiteral(SETTING_TIMER_EXPEDITION_CLOSE), m_timerdata.expeditionClose());
    m_settings->setValue(QStringLiteral(SETTING_TIMER_CONSTRUCTION_CLOSE), m_timerdata.constructionClose());
    m_settings->endGroup();

    //ウインドウの位置を保存
    m_settings->beginGroup(QStringLiteral(SETTING_TIMERDIALOG));
    m_settings->setValue(QStringLiteral(SETTING_WINDOW_GEO), saveGeometry());
    m_settings->endGroup();
}

void TimerDialog::closeEvent(QCloseEvent *event)
{
    //設定保存
    saveSettings();

    QDialog::closeEvent(event);
}


