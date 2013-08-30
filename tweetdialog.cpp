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
#include "tweetdialog.h"
#include "ui_tweetdialog.h"

#include <QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtCore/QDebug>


TweetDialog::TweetDialog(QWidget *parent) :
    QDialog(parent),
    m_imageLabel(this),
    m_oauth(this),
    m_status(this),
    ui(new Ui::TweetDialog)
{
    ui->setupUi(this);

    //Ctrl+Enterで送信
    ui->tweetTextEdit->addAction(ui->actionTweet);
    connect(ui->actionTweet, &QAction::triggered, ui->tweetButton, &QPushButton::clicked);
    //キャプチャ画像表示
    m_imageLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_imageLabel.setGeometry(QRect(70, 20, 250, 150));
    //oAuth
    connect(&m_oauth, SIGNAL(stateChanged(OAuth::State)), this, SLOT(stateChanged(OAuth::State)));
    m_oauth.setConsumerKey(QString(TWITTER_CONSUMER_KEY));
    m_oauth.setConsumerSecret(QString(TWITTER_CONSUMER_SECRET));
}

TweetDialog::~TweetDialog()
{
    delete ui;
}

QString TweetDialog::imagePath() const
{
    return m_imagePath;
}
void TweetDialog::setImagePath(const QString &imagePath)
{
    if(m_imagePath.compare(imagePath) == 0)
        return;

    m_imagePath = imagePath;

    //表示画像を変更
    m_imagePixmap.load(imagePath);
    m_imageLabel.setPixmap(m_imagePixmap.scaled(m_imageLabel.size(),
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation));

}

const QString &TweetDialog::token() const
{
    return m_oauth.token();
}

void TweetDialog::setToken(const QString &token)
{
    m_oauth.setToken(token);
}

const QString &TweetDialog::tokenSecret() const
{
    return m_oauth.tokenSecret();
}

void TweetDialog::setTokenSecret(const QString &tokenSecret)
{
    m_oauth.setTokenSecret(tokenSecret);
}

const QString &TweetDialog::user_id() const
{
    return m_oauth.user_id();
}

void TweetDialog::user_id(const QString &user_id)
{
    m_oauth.user_id(user_id);
}

const QString &TweetDialog::screen_name() const
{
    return m_oauth.screen_name();
}

void TweetDialog::screen_name(const QString &screen_name)
{
   m_oauth.screen_name(screen_name);
   if(screen_name.length() > 0)
       ui->userIdLabel->setText(tr("Tweet by %1").arg(screen_name));
   else
       ui->userIdLabel->setText(tr("Tweet by %1").arg("unknown"));
}


//閉じる
void TweetDialog::on_closeButton_clicked()
{
    close();
}

//つぶやくボタン
void TweetDialog::on_tweetButton_clicked()
{
    //認証済みか確認
    if(user_id().length() == 0)
        return;

    //画像が指定されてるか
    if(!QFile::exists(imagePath()))
        return;

    //画像のバイナリデータを読み込む
    QFile file(imagePath());
    if(!file.open(QIODevice::ReadOnly))
        return;
    QDataStream in(&file);
    QByteArray imagedata(in.device()->readAll());
    file.close();

//    qDebug() << "image:" << imagedata.length() << "," << imagePath();

    //つぶやく
    QVariantMap map;
    QVariantList list;
    list.append(imagedata);
    QStringList slist;
    slist.append(imagePath());
    map.insert("status", ui->tweetTextEdit->document()->toPlainText());
    map.insert("media", slist);
    m_status.statusesUpdate(map);

    //消す
    ui->tweetTextEdit->clear();
    setImagePath("");
    close();
}


//つぶやきの入力内容が変化した
void TweetDialog::on_tweetTextEdit_textChanged()
{
    int len = ui->tweetTextEdit->document()->toPlainText().length();
    ui->charCountLabel->setText(QString("%1").arg(140 - len));
    if(len > 0)
        ui->tweetButton->setEnabled(true);
    else
        ui->tweetButton->setEnabled(false);
}

//ダイアログの実行開始
int TweetDialog::exec()
{
    //認証済みか確認
    authorize();

    //ハッシュタグ


    return QDialog::exec();
}


//OAuthの状態が変わった
void TweetDialog::stateChanged(OAuth::State state)
{
    switch(state){
    case OAuth::RequestTokenReceived:
    {
//        qDebug() << "RequestTokenReceived:";
//        qDebug() << "   token:" << m_oauth.token();
//        qDebug() << "   tokenSecret:" << m_oauth.tokenSecret();

        //PINを取得しにブラウザを開く
        m_oauth.authorize();

        //PIN入力のダイアログを表示
        QString pin = QInputDialog::getText(this, tr("Authorization"), tr("Please input pin code."));
        //PINで最終認証
        if(!pin.isEmpty())
            m_oauth.access_token(pin);

        break;
    }
    case OAuth::Authorized:
//        qDebug() << "Authorized";
//        qDebug() << "   token:" << m_oauth.token();
//        qDebug() << "   tokenSecret:" << m_oauth.tokenSecret();
//        qDebug() << "   user_id:" << m_oauth.user_id();
//        qDebug() << "   screen_name:" << m_oauth.screen_name();

        //本当はあまり意味ないけど表示の更新
        screen_name(m_oauth.screen_name());

        break;
    default:
        break;
    }

    //認証してないと使えないエディット・ボタン
    if(state == OAuth::Authorized){
        ui->tweetButton->setEnabled(true);
        ui->reauthButton->setEnabled(true);
    }else{
        ui->tweetButton->setEnabled(false);
        ui->reauthButton->setEnabled(false);
    }
}

//認証チェックなど
void TweetDialog::authorize(bool force)
{
    //認証済みか確認
    if(user_id().length() == 0 || force){
        //未認証
        QMessageBox::information(this
                                 , tr("Information")
                                 , tr("Do the authentication of Twitter."));
        //認証開始
        m_oauth.unauthorize();
        m_oauth.request_token("");
    }
}

//再認証ボタン
void TweetDialog::on_reauthButton_clicked()
{
    authorize(true);
}

