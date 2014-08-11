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
#ifndef UPDATEINFODIALOG_H
#define UPDATEINFODIALOG_H

#include <QDate>
#include <QDialog>
#include <QSettings>

namespace Ui {
class UpdateInfoDialog;
}

class UpdateInfoDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit UpdateInfoDialog(QWidget *parent = 0
                            , QSettings *settings = NULL);
    ~UpdateInfoDialog();
    
    void CheckUpdate();

    QString lastFavoriteUpdateDate() const;
    void setLastFavoriteUpdateDate(const QString &lastFavoriteUpdateDate);
    QString lastTimerSelectGuideUpdateDate() const;
    void setLastTimerSelectGuideUpdateDate(const QString &lastTimerSelectGuideUpdateDate);
    QString lastRecognizeInfoUpdateDate() const;
    void setLastRecognizeInfoUpdateDate(const QString &lastRecognizeInfoUpdateDate);

signals:
    void lastFavoriteUpdateDateChanged(const QString &lastFavoriteUpdateDate);
    void lastTimerSelectGuideUpdateDateChanged(const QString &lastTimerSelectGuideUpdateDate);
    void lastRecognizeInfoUpdateDateChanged(const QString &lastRecognizeInfoUpdateDate);

private:
    Ui::UpdateInfoDialog *ui;

    QSettings *m_settings;
    int m_hideVersionCode;      //非表示にしたいバージョンコード
    int m_newVersionCode;       //ダウンロードして取得したバージョンコード
    int m_force;                //強制表示

    QString m_lastFavoriteUpdateDate;
    QString m_lastTimerSelectGuideUpdateDate;
    QString m_lastRecognizeInfoUpdateDate;

    bool isHide(int checkVersion);
};

#endif // UPDATEINFODIALOG_H
