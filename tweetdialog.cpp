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

#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtTwitterAPI/oauth.h>
#include <QtTwitterAPI/status.h>

#include "../KanmusuMemoryTwitter/twitterinfo.h"

class TweetDialog::Private
{
public:
    Private(TweetDialog *parent);

private:
    TweetDialog *q;
    Ui::TweetDialog ui;

public:
    OAuth oauth;
    QString imagePath;
};

TweetDialog::Private::Private(TweetDialog *parent)
    : q(parent)
{
    ui.setupUi(q);

    //表示画像を変更
    connect(q, &TweetDialog::imagePathChanged, [this](const QString &imagePath) {
        qreal w = ui.screenshot->width();
        qreal h = ui.screenshot->height();
        QPixmap pixmap(imagePath);
        ui.screenshot->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });

    //Ctrl+Enterで送信
    ui.tweetTextEdit->addAction(ui.actionTweet);
    connect(ui.actionTweet, &QAction::triggered, ui.tweetButton, &QPushButton::animateClick);

    connect(ui.reauthButton, &QPushButton::clicked, [this]() {
        //認証開始
        oauth.unauthorize();
        oauth.request_token();
    });

    connect(ui.tweetTextEdit, &QPlainTextEdit::textChanged, [this]() {
        //つぶやきの入力内容が変化した
        int len = ui.tweetTextEdit->toPlainText().length();
        ui.charCountLabel->setText(QString("%1").arg(117 - len));
        ui.tweetButton->setEnabled(len > 0);
    });
    ui.tweetTextEdit->setPlainText(tr(" #kancolle"));

    connect(ui.tweetButton, &QPushButton::clicked, [this]() {
        //認証済みか確認
        if (oauth.state() != OAuth::Authorized) return;

        //画像が指定されてるか
        if(!QFile::exists(imagePath)) return;

        Status *status = new Status(q);
        connect(status, &Status::loadingChanged, [this](bool loading) {
            if (loading) {
                q->setEnabled(false);
                ui.tweetButton->setText(tr("Sending"));
            } else {
                //消す
                q->accept();
            }
        });
        QVariantMap map;
        map.insert("status", ui.tweetTextEdit->toPlainText());
        map.insert("media", QStringList() << imagePath);
        status->statusesUpdate(map);
    });

    connect(ui.closeButton, &QPushButton::clicked, [this]() {
        q->close();
    });

    //OAuth
    connect(&oauth, &OAuth::stateChanged, [this](OAuth::State state) {
        switch (state) {
        case OAuth::RequestTokenReceived: {
    //        qDebug() << "RequestTokenReceived:";
    //        qDebug() << "   token:" << m_oauth.token();
    //        qDebug() << "   tokenSecret:" << m_oauth.tokenSecret();

            //PINを取得しにブラウザを開く
            oauth.authorize();

            //PIN入力のダイアログを表示
            QString pin = QInputDialog::getText(q, tr("Authorization"), tr("Please input pin code."));
            //PINで最終認証
            if(pin.isEmpty()) {
            } else {
                oauth.access_token(pin);
            }
            break; }
        default:
            break;
        }

        ui.tweetButton->setEnabled(state == OAuth::Authorized);
//        ui.reauthButton->setEnabled(state == OAuth::Authorized || state == OAuth::Unauthorized);
    });
    connect(&oauth, &OAuth::tokenChanged, q, &TweetDialog::tokenChanged);
    connect(&oauth, &OAuth::tokenSecretChanged, q, &TweetDialog::tokenSecretChanged);
    connect(&oauth, &OAuth::user_idChanged, q, &TweetDialog::user_idChanged);
    connect(&oauth, &OAuth::screen_nameChanged, q, &TweetDialog::screen_nameChanged);
    oauth.setConsumerKey(QStringLiteral(TWITTER_CONSUMER_KEY));
    oauth.setConsumerSecret(QStringLiteral(TWITTER_CONSUMER_SECRET));

    connect(q, &TweetDialog::screen_nameChanged, [this](const QString &screen_name) {
        ui.screenName->setVisible(!screen_name.isEmpty());
        ui.screenName->setText(QStringLiteral("@%1").arg(screen_name));
    });
    ui.screenName->setVisible(!oauth.screen_name().isEmpty());
    ui.screenName->setText(QStringLiteral("@%1").arg(oauth.screen_name()));

    ui.name->setVisible(false);
    ui.avatar->setVisible(false);
    ui.tweetTextEdit->setFocus();
}

TweetDialog::TweetDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Private(this))
{
    connect(this, &QObject::destroyed, [this]() { delete d; });
}

void TweetDialog::showEvent(QShowEvent *event)
{
    if (d->oauth.state() != OAuth::Authorized) {
        //未認証
        QMessageBox::StandardButton res = QMessageBox::information(this
                                 , tr("Information")
                                 , tr("Do the authentication of Twitter.")
                                 , QMessageBox::Ok | QMessageBox::Cancel);
        if(res == QMessageBox::Ok)
            d->oauth.request_token();
    }
    QDialog::showEvent(event);
}

const QString &TweetDialog::imagePath() const
{
    return d->imagePath;
}

void TweetDialog::setImagePath(const QString &imagePath)
{
    if (d->imagePath == imagePath) return;
    d->imagePath = imagePath;
    emit imagePathChanged(imagePath);
}

const QString &TweetDialog::token() const
{
    return d->oauth.token();
}

void TweetDialog::setToken(const QString &token)
{
    d->oauth.setToken(token);
}

const QString &TweetDialog::tokenSecret() const
{
    return d->oauth.tokenSecret();
}

void TweetDialog::setTokenSecret(const QString &tokenSecret)
{
    d->oauth.setTokenSecret(tokenSecret);
}

const QString &TweetDialog::user_id() const
{
    return d->oauth.user_id();
}

void TweetDialog::user_id(const QString &user_id)
{
    d->oauth.user_id(user_id);
}

const QString &TweetDialog::screen_name() const
{
    return d->oauth.screen_name();
}

void TweetDialog::screen_name(const QString &screen_name)
{
   d->oauth.screen_name(screen_name);
}
