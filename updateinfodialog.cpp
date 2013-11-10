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
#include "updateinfodialog.h"
#include "ui_updateinfodialog.h"

#include "kanmusumemory_global.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QDebug>

#define CHECK_UPDATE_URL    QString("http://157.7.154.101:8080/kanmemo/api/checkupdate.qml?version=%1+versioncode=%2+os=%3+lang=%4")

// OS number define
// 0: Windows(32bit)
// 1: Windows(64bit)
// 2: Mac OS X(32bit)
// 3: Mac OS X(64bit)
// 4: Linux(32bit)
// 5: Linux(64bit)
#define OS_NUM_WIN_32       0
#define OS_NUM_WIN_64       1
#define OS_NUM_MAC_32       2
#define OS_NUM_MAC_64       3
#define OS_NUM_LINUX_32     4
#define OS_NUM_LINUX_64     5

#define KEY_DOWNLOAD_URL        QStringLiteral("download-url")
#define KEY_WEBPAGE_URL         QStringLiteral("webpage-url")
#define KEY_NEW_VERSION         QStringLiteral("new-version")
#define KEY_NEW_VERSION_CODE    QStringLiteral("new-version-code")
#define KEY_UPDATE_EXIST        QStringLiteral("update-exist")


UpdateInfoDialog::UpdateInfoDialog(QWidget *parent
                                   , QSettings *settings) :
    QDialog(parent),
    m_settings(settings),
    m_newVersionCode(0),
    m_force(false),
    ui(new Ui::UpdateInfoDialog)
{
    ui->setupUi(this);

    //OKボタンを押した時
    connect(ui->buttonBox, &QDialogButtonBox::accepted, [this](){
        if(m_settings != NULL){
            m_settings->beginGroup(SETTING_UPDATE_CHECK);
            //次回まで非表示するか
            m_settings->setValue(SETTING_UPDATE_CHECK_HIDE, ui->hideNextUpdatecheckBox->isChecked());
            //非表示対象のバージョン
            if(ui->hideNextUpdatecheckBox->isChecked()){
                m_hideVersionCode = m_newVersionCode;
            }else{
                m_hideVersionCode = 0;
            }
            m_settings->setValue(SETTING_UPDATE_CHECK_HIDE_VERSION, m_hideVersionCode);

            m_settings->endGroup();
        }
        accept();
    });

    if(m_settings != NULL){
        m_settings->beginGroup(SETTING_UPDATE_CHECK);
        //次回まで非表示するか
        ui->hideNextUpdatecheckBox->setChecked(m_settings->value(SETTING_UPDATE_CHECK_HIDE, false).toBool());
        //非表示対象のバージョン
        m_hideVersionCode = m_settings->value(SETTING_UPDATE_CHECK_HIDE_VERSION, 0).toInt();
        m_settings->endGroup();
    }
}

UpdateInfoDialog::~UpdateInfoDialog()
{
    delete ui;
}

void UpdateInfoDialog::CheckUpdate()
{
    int os_num = 0;

#if defined(Q_OS_WIN32)
    if(QT_POINTER_SIZE == 4){
        os_num = OS_NUM_WIN_32;
    }else{
        os_num = OS_NUM_WIN_64;
    }
#elif defined(Q_OS_MAC)
    if(QT_POINTER_SIZE == 4){
        os_num = OS_NUM_MAC_32;
    }else{
        os_num = OS_NUM_MAC_64;
    }
#else
    if(QT_POINTER_SIZE == 4){
        os_num = OS_NUM_LINUX_32;
    }else{
        os_num = OS_NUM_LINUX_64;
    }
#endif

//    qDebug() << "check update";
    QNetworkAccessManager *net = new QNetworkAccessManager(this);
    connect(net, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
        if(reply->error() == QNetworkReply::NoError){

            QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
            bool exist = json.object().value(KEY_UPDATE_EXIST).toBool();

//            qDebug() << "download " << json.object().value(KEY_DOWNLOAD_URL).toString();
//            qDebug() << "web " << json.object().value(KEY_WEBPAGE_URL).toString();
//            qDebug() << "exist " << exist;
//            qDebug() << "ver " << json.object().value(KEY_NEW_VERSION).toString();
//            qDebug() << "ver code " << json.object().value(KEY_NEW_VERSION_CODE).toDouble() << "<=" << KANMEMO_VERSION_CODE;

            //バージョンコード
            m_newVersionCode = static_cast<int>(json.object().value(KEY_NEW_VERSION_CODE).toDouble());
            if(isHide(m_newVersionCode) && !m_force){
                m_force = true; //1度通ったらはユーザー操作なので強制表示
                return; //非表示
            }else{
                m_force = true; //1度通ったらはユーザー操作なので強制表示
            }

            if(exist){
                //ダイアログ表示
                //Download
                QString download_url = json.object().value(KEY_DOWNLOAD_URL).toString();
                QStringList url_list = download_url.split("|");
                ui->downloadUrlLabel1->setText(QString("<a href='%1'>%1</a>").arg(url_list[0]));
                if(url_list.count() > 1){
                    ui->downloadUrlLabel2->setText(QString("<a href='%1'>%1</a>").arg(url_list[1]));
                }else{
                    ui->downloadUrlLabel2->clear();
                }
                //WebPage
                ui->webpageUrlLabel->setText(QString("<a href='%1'>%1</a>").arg(json.object().value(KEY_WEBPAGE_URL).toString()));
                //Version Number
                ui->versionNumberLabel->setText(json.object().value(KEY_NEW_VERSION).toString());

                show();
            }
        }
    });
    //プロキシ
    if(m_settings != NULL){
        QNetworkProxy *proxy = new QNetworkProxy();
        bool enable = m_settings->value(SETTING_GENERAL_PROXY_ENABLE, false).toBool();
        QString host = m_settings->value(SETTING_GENERAL_PROXY_HOST).toString();
        if(host.length() > 0 && enable){
            proxy->setType(QNetworkProxy::HttpProxy);
            proxy->setHostName(host);
            proxy->setPort(m_settings->value(SETTING_GENERAL_PROXY_PORT, 8888).toInt());

            net->setProxy(*proxy);
        }else{
            net->setProxy(QNetworkProxy::NoProxy);
        }
    }
    //アクセス開始
    net->get(QNetworkRequest(CHECK_UPDATE_URL
                             .arg(KANMEMO_VERSION)
                             .arg(KANMEMO_VERSION_CODE)
                             .arg(os_num)
                             .arg(QLocale::system().name())));
}
//非表示か確認
bool UpdateInfoDialog::isHide(int checkVersionCode)
{
    bool ret = false;

    if(ui->hideNextUpdatecheckBox->isChecked()){
        if(checkVersionCode <= m_hideVersionCode){
            ret = true;
        }else{
            ret = false;
        }
    }
//    qDebug() << "hide:" << ret << "," << checkVersionCode << "<=" << m_hideVersionCode;
    return ret;
}
