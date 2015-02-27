/*
 * Copyright 2013-2015 KanMemo Project.
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
#include <QSettings>

class TweetDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TweetDialog(QWidget *parent = 0, QSettings *settings = NULL);
    ~TweetDialog();

    const QString &token() const;
    const QString &tokenSecret() const;
    const QString &user_id() const;
    const QString &screen_name() const;
    const QStringList &getImagePathList() const;

    void addImagePath(const QString &path);
    void clearImagePath();
    void removeImagePath(int i);

    void saveSettings();

public slots:
    void setToken(const QString &token);
    void setTokenSecret(const QString &tokenSecret);
    void user_id(const QString &user_id);
    void screen_name(const QString &screen_name);
    void setImagePathList(const QStringList &value);

signals:
    void imagePathChanged(const QString &imagePath);
    void tokenChanged(const QString &token);
    void tokenSecretChanged(const QString &tokenSecret);
    void user_idChanged(const QString &user_id);
    void screen_nameChanged(const QString &screen_name);
    void imagePathListChanged(const QStringList &imagePathList);

    void triggeredCapture();
    void triggeredCaptureAndEdit();
    void triggeredViewMemories();
    void triggeredNotificationTimer();
    void triggeredCaptureFleetDetail();
    void triggeredCaptureFleetList();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private:
    class Private;
    Private *d;
};

#endif // TWEETDIALOG_H
