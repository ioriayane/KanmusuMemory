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
#include <QPainter>

#include "clickablelabel.h"
#include "twitterinfo.h"

class TweetDialog::Private
{
public:
    Private(TweetDialog *parent);

    enum SendMethod {
        None
        , SinglePost
        , MultiPost
    };

private:
    TweetDialog *q;
    Ui::TweetDialog ui;

    bool existImages();
    bool hitRemoveMark(const ClickableLabel *label, const QRect &rect, const QPoint &point);
    void clickedRemoveMark(const ClickableLabel *label, int index, const QPoint &point);

    SendMethod sendmethod;
    QStringList mediaIds;
    QList<QRect> previewImageRect;  //ぺけマークの当たり判定用

public slots:
    void updatePreviewImages(const QStringList &imagePathList);

public:
    bool sending() {   return sendmethod != SendMethod::None;  }
    OAuth oauth;
    Status status;
    QString imagePath;
    QStringList imagePathList;
};

TweetDialog::Private::Private(TweetDialog *parent)
    : q(parent)
    , sendmethod(SendMethod::None)
{
    ui.setupUi(q);

    //表示画像を変更
    connect(q, &TweetDialog::imagePathChanged, [this](const QString &imagePath) {
        qreal w = ui.screenshot->width();
        qreal h = ui.screenshot->height();
        QPixmap pixmap(imagePath);
        ui.screenshot->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });

    //表示画像変更
    connect(q, &TweetDialog::imagePathListChanged, [this](const QStringList &imagePathList){
        updatePreviewImages(imagePathList);
        if(imagePathList.length() == 0){
            q->reject();
        }
    });

    //Ctrl+Enterで送信
    ui.tweetTextEdit->addAction(ui.actionTweet);
    connect(ui.actionTweet, &QAction::triggered, ui.tweetButton, &QPushButton::animateClick);

    //ファンクションキー
    ui.tweetTextEdit->addAction(ui.actionCapture);              //f5
    ui.tweetTextEdit->addAction(ui.actionCaptureAndEdit);       //ctrl + f5
    ui.tweetTextEdit->addAction(ui.actionViewMemories);         //f6
    ui.tweetTextEdit->addAction(ui.actionNotificationTimer);    //f7
    ui.tweetTextEdit->addAction(ui.actionCaptureFleetDetail);   //f9
    ui.tweetTextEdit->addAction(ui.actionCaptureFleetList);     //f10
    connect(ui.actionCapture, &QAction::triggered, [this]{ emit q->triggeredCapture(); });
    connect(ui.actionCaptureAndEdit, &QAction::triggered, [this]{ emit q->triggeredCaptureAndEdit(); });
    connect(ui.actionViewMemories, &QAction::triggered, [this]{ emit q->triggeredViewMemories(); });
    connect(ui.actionNotificationTimer, &QAction::triggered, [this]{ emit q->triggeredNotificationTimer(); });
    connect(ui.actionCaptureFleetDetail, &QAction::triggered, [this]{ emit q->triggeredCaptureFleetDetail(); });
    connect(ui.actionCaptureFleetList, &QAction::triggered, [this]{ emit q->triggeredCaptureFleetList(); });


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

    //送信状態の変化
    connect(&status, &Status::loadingChanged, [this](bool loading) {
        if (loading) {
            q->setEnabled(false);
            ui.tweetButton->setText(tr("Sending"));
        } else {
            //完了
            if(sendmethod == SendMethod::MultiPost){
                //取得したIDを保存
                mediaIds.append(status.data().value("media_id_string", "").toString());
                qDebug() << "multipost:1.5:" << status.data().value("media_id_string", "").toString();
                //次の動作
                if(imagePathList.length() > 0){
                    //次の画像
                    qDebug() << "multipost:2+:" << imagePathList.front();
                    sendmethod = SendMethod::MultiPost;
                    QVariantMap map;
                    map.insert("media", QStringList() << imagePathList.front());
                    imagePathList.pop_front();
                    status.mediaUpload(map);
                }else{
                    //全て投げたので呟く
                    qDebug() << "multipost:3:" << mediaIds.join(',');
                    sendmethod = SendMethod::SinglePost;
                    QVariantMap map;
                    map.insert("status", ui.tweetTextEdit->toPlainText());
                    map.insert("media_ids", mediaIds.join(','));
                    status.statusesUpdate(map);
                }
            }else{
                //状態を戻す
                imagePathList.clear();
                mediaIds.clear();
                ui.tweetTextEdit->setPlainText(tr(" #kancolle"));
                q->setEnabled(true);
                ui.tweetButton->setText(tr("Tweet"));
                sendmethod = SendMethod::None;
                //閉じる
                q->accept();
            }
        }
    });

    //送信ボタン
    connect(ui.tweetButton, &QPushButton::clicked, [this]() {
        //認証済みか確認
        if (oauth.state() != OAuth::Authorized) return;

        //画像が指定されてるか
        if(!existImages())  return;

        if(imagePathList.length() == 1){
            //画像１枚の時
            sendmethod = SendMethod::SinglePost;
            QVariantMap map;
            map.insert("status", ui.tweetTextEdit->toPlainText());
            map.insert("media", QStringList() << imagePathList.front());
            imagePathList.pop_front();
            status.statusesUpdate(map);
        }else{
            //画像が複数の時
            qDebug() << "multipost:1:" << imagePathList.front();
            mediaIds.clear();
            sendmethod = SendMethod::MultiPost;
            QVariantMap map;
            map.insert("media", QStringList() << imagePathList.front());
            imagePathList.pop_front();
            status.mediaUpload(map);
        }
    });

    //閉じるボタン
    connect(ui.closeButton, &QPushButton::clicked, [this]() {
        q->reject();
    });

    //プレビュー画像をクリック
    connect(ui.screenshot, &ClickableLabel::released, [this](QMouseEvent *ev){
        clickedRemoveMark(ui.screenshot, 0, ev->pos());
    });
    connect(ui.screenshot2, &ClickableLabel::released, [this](QMouseEvent *ev){
        clickedRemoveMark(ui.screenshot2, 1, ev->pos());
    });
    connect(ui.screenshot3, &ClickableLabel::released, [this](QMouseEvent *ev){
        clickedRemoveMark(ui.screenshot3, 2, ev->pos());
    });
    connect(ui.screenshot4, &ClickableLabel::released, [this](QMouseEvent *ev){
        clickedRemoveMark(ui.screenshot4, 3, ev->pos());
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

bool TweetDialog::Private::existImages()
{
    bool ret = true;
    if(imagePathList.length() == 0){
        ret = false;
    }else{
        foreach (QString path, imagePathList) {
            if(!QFile::exists(path)){
                ret = false;
            }
        }
    }
    return ret;
}

bool TweetDialog::Private::hitRemoveMark(const ClickableLabel *label, const QRect &rect, const QPoint &point)
{
    qDebug() << QString("hit? tw=%3,th=%4, iw=%5, ih=%6, x=%1, y=%2").arg(point.x()).arg(point.y())
                .arg(label->width()).arg(label->height())
                .arg(rect.width()).arg(rect.height());
    qreal wc = label->width() / 2;
    qreal image_right = wc + rect.width() / 2;
    qreal mark_left = image_right - 24;
    qreal hc = label->height() / 2;
    qreal image_top = hc - rect.height() / 2;
    qreal mark_bottom = image_top + 24;
    qDebug() << QString("     wc=%1, image_right=%2, mark_left=%3, hc=%4, image_top=%5, mark_bottom=%6")
                .arg(wc).arg(image_right).arg(mark_left).arg(hc).arg(image_top).arg(mark_bottom);
    if(mark_left <= point.x() && point.x() <= image_right
            && image_top <= point.y() && point.y() <= mark_bottom){
        return true;
    }else{
        return false;
    }
}

void TweetDialog::Private::clickedRemoveMark(const ClickableLabel *label, int index, const QPoint &point)
{
    if(previewImageRect.length() > index){
        if(hitRemoveMark(label, previewImageRect.at(index), point)){
            q->removeImagePath(index);
            qDebug() << "HIT " << index;
        }
    }
}

void TweetDialog::Private::updatePreviewImages(const QStringList &imagePathList)
{
    QLabel *screenshot[] = {ui.screenshot, ui.screenshot2, ui.screenshot3, ui.screenshot4};
    QImage mark(":/resources/ImageRemoveMark.png");
    qreal total_w = ui.gridLayout->contentsRect().width();
    qreal total_h = ui.gridLayout->contentsRect().height();
    qreal w;
    qreal h;
    previewImageRect.clear();
    for(int i=0; i<4; i++){
        if(imagePathList.length() > 1){
            w = total_w / 2;
        }else{
            w = total_w;
        }
        if(imagePathList.length() == 2){
            h = total_h;
        }else{
            h = w * 0.6;
        }
        if(i < imagePathList.length()){
            QImage img(imagePathList.at(i));
            QImage img2(img.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            QPainter painter(&img2);
            painter.drawImage(img2.width() - mark.width() - 1, 1, mark);
            screenshot[i]->setFixedWidth(static_cast<int>(w));
            screenshot[i]->setFixedHeight(static_cast<int>(h));
            screenshot[i]->setMaximumSize(w, h);
            screenshot[i]->setPixmap(QPixmap::fromImage(img2));
            screenshot[i]->setVisible(true);
            //ぺけマークの当たり判定用に保存しておく
            previewImageRect.append(img2.rect());
        }else{
            screenshot[i]->setVisible(false);
        }
    }
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

    //最初は幅がとれないので
    d->updatePreviewImages(d->imagePathList);
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

const QStringList &TweetDialog::getImagePathList() const
{
    return d->imagePathList;
}

void TweetDialog::setImagePathList(const QStringList &value)
{
    d->imagePathList = value;
    emit imagePathListChanged(value);
}

void TweetDialog::addImagePath(const QString &path)
{
    if(d->imagePathList.length() >= 4 || d->sending())  return;
    d->imagePathList.append(path);
    emit imagePathListChanged(d->imagePathList);
}

void TweetDialog::clearImagePath()
{
    d->imagePathList.clear();
    emit imagePathListChanged(d->imagePathList);
}

void TweetDialog::removeImagePath(int i)
{
    if(i < 0 || i >= d->imagePathList.length())  return;
    d->imagePathList.removeAt(i);
    emit imagePathListChanged(d->imagePathList);
}
