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
#include "timerdialog.h"
#include "ui_timerdialog.h"
#include "kanmusumemory_global.h"
#include "twitterinfo.h"

#include <QQmlContext>
#include <QtQml/QQmlEngine>
#include <QDateTime>

#include <QtCore/QDebug>


TimerDialog::TimerDialog(QWidget *parent) :
    QDialog(parent),
    m_viewer(NULL),
    trayIcon(NULL),
    settings(NULL),
    m_oauth(this),
    m_status(this),
    ui(new Ui::TimerDialog)
{
    ui->setupUi(this);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));

    m_timer.start(10000);
}

TimerDialog::~TimerDialog()
{
    if(m_viewer != NULL)
        delete m_viewer;
    delete ui;
}

void TimerDialog::resizeEvent(QResizeEvent *event)
{
    if(m_viewer != NULL){
        m_viewer->setGeometry(QRect(0, 0, width(), height()));
    }
}

void TimerDialog::showEvent(QShowEvent *event)
{
    if(m_viewer == NULL){
        m_viewer = new QtQuick2ApplicationViewer(windowHandle());
        connect(m_viewer->engine(), SIGNAL(quit()), this, SLOT(closeQml()));

        //この辺りで途中状態の読み込み？

        //C++のデータをQML側へ公開
        m_viewer->rootContext()->setContextProperty("timerData", &m_timerdata);

        //QML設定して表示
        m_viewer->setSource(QUrl("qrc:///qml/KanmusuMemory/timerDialog.qml"));
        m_viewer->show();
    }
}

void TimerDialog::closeQml()
{
//    accept();
}

//全体のカウント処理
void TimerDialog::timeout()
{
    QStringList messages;
    QStringList numbername;
    numbername << tr("1st") << tr("2nd") << tr("3rd") << tr("4th") << tr("5th") << tr("6th") << tr("7th") << tr("8th");

    //入渠

    for(int i=0; i<m_timerdata.dockingRunning().length(); i++){
        if(m_timerdata.dockingRunning()[i]){
//            qDebug() << i << ":set=" << m_timerdata.dockingTime()[i]
//                        << ",start=" << m_timerdata.dockingStart()[i];
            if(checkKanmemoTimerTimeout(m_timerdata.dockingTime()[i]
                                        , m_timerdata.dockingStart()[i])){
//                qDebug() << "doc:" << i << ":" << "timeout";
                m_timerdata.setRunning(0, i, false);

                //メッセージ追加
                messages.append(tr("It is time that Kanmusu is up from the %1 bath.").arg(numbername[i]));
//                messages.append(tr("ドック%1の艦娘がお風呂から上がる頃です。").arg(i+1));
            }
        }
    }
    //遠征
    for(int i=0; i<m_timerdata.expeditionRunning().length(); i++){
        if(m_timerdata.expeditionRunning()[i]){
            if(checkKanmemoTimerTimeout(m_timerdata.expeditionTime()[i]
                                        , m_timerdata.expeditionStart()[i])){
//                qDebug() << "exp:" << i << ":" << "timeout";
                m_timerdata.setRunning(1, i, false);

                //メッセージ追加
                messages.append(tr("It is time the %1 Fleet is returned.").arg(numbername[i+1]));
            }
        }
    }
    //建造
    for(int i=0; i<m_timerdata.constructionRunning().length(); i++){
        if(m_timerdata.constructionRunning()[i]){
            if(checkKanmemoTimerTimeout(m_timerdata.constructionTime()[i]
                                        , m_timerdata.constructionStart()[i])){
//                qDebug() << "con:" << i << ":" << "timeout";
                m_timerdata.setRunning(2, i, false);

                //メッセージ追加
                messages.append(tr("It is time the Kanmusu of %1 dock is completed.").arg(numbername[i]));
            }
        }
    }

    if(messages.length() > 0){
        //システムトレイ
        QString msg;
        for(int i=0; i<messages.length(); i++){
            msg.append(messages[i] + "\n");
        }
        trayIcon->showMessage(tr("KanMemo"), msg
                              , QSystemTrayIcon::Information, 5000);

        //つぶやく
        tweetTimerMessage(messages);
    }

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

//つぶやく
void TimerDialog::tweetTimerMessage(const QStringList &messages)
{
    if(settings == NULL)
        return;

    m_oauth.setConsumerKey(TWITTER_CONSUMER_KEY);
    m_oauth.setConsumerSecret(TWITTER_CONSUMER_SECRET);
    m_oauth.setToken(settings->value(SETTING_GENERAL_TOKEN, "").toString());
    m_oauth.setTokenSecret(settings->value(SETTING_GENERAL_TOKENSECRET, "").toString());
    m_oauth.user_id(settings->value(SETTING_GENERAL_USER_ID, "").toString());
    m_oauth.screen_name(settings->value(SETTING_GENERAL_SCREEN_NAME, "").toString());

    if(m_oauth.state() == OAuth::Authorized){
        qDebug() << "認証済み";
        QString message;

        message = "@" + m_oauth.screen_name() + " ";
        for(int i=0; i<messages.length(); i++){
            QString temp = messages[i];

            if((message.length() + temp.length()) >= 130){
                //文字数オーバー一旦送信
//                qDebug() << message.length() << "," << message;
                QVariantMap map;
                map.insert("status", message);
                m_status.updateStatuses(map);

                //消す
                message = "@" + m_oauth.screen_name() + " ";
                message.append(temp + "\n ");
            }else{
                message.append(temp + "\n ");
            }
        }
//        qDebug() << message.length() << "," << message;
        QVariantMap map;
        map.insert("status", message);
        m_status.updateStatuses(map);
    }
}


