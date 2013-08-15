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
#ifndef TWEETDIALOG_H
#define TWEETDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPixmap>
#include <QtTwitterAPI>

#include "twitterinfo.h"

namespace Ui {
class TweetDialog;
}

class TweetDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TweetDialog(QWidget *parent = 0);
    ~TweetDialog();
    
    QString imagePath() const;
    void setImagePath(const QString &imagePath);

    const QString &token() const;
    void setToken(const QString &token);

    const QString &tokenSecret() const;
    void setTokenSecret(const QString &tokenSecret);

    const QString &user_id() const;
    void user_id(const QString &user_id);

    const QString &screen_name() const;
    void screen_name(const QString &screen_name);

private slots:
    void on_closeButton_clicked();
    void on_tweetButton_clicked();
    void on_tweetTextEdit_textChanged();
    void on_reauthButton_clicked();


public slots:
    virtual int exec();
    void stateChanged(OAuth::State state);

private:
    Ui::TweetDialog *ui;

    QLabel m_imageLabel;
    QPixmap m_imagePixmap;

    QString m_imagePath;

    //ツイッター関連
    OAuth m_oauth;
    Status m_status;

    void authorize(bool force = false);

};

#endif // TWEETDIALOG_H
