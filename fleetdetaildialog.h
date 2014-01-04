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
#ifndef FLEETDETAILDIALOG_H
#define FLEETDETAILDIALOG_H

#include "webview.h"

#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QDialog>

namespace Ui {
class FleetDetailDialog;
}

class CaptureInterface : public QObject
{
    Q_OBJECT
public:
    WebView *webview;

    Q_INVOKABLE QString captureGame(qreal x, qreal y, qreal w, qreal h);
};

class FleetDetailDialog : public QDialog
{
    Q_OBJECT

public:
    enum NextOperationType {
        None
        , Combine
    };

    explicit FleetDetailDialog(WebView *webview
                               , QRect capture_rect     //取り込み範囲
                               , qreal view_ratio       //プレビューの倍率
                               , int columns            //列数
                               , int max                //取り込み最大数
                               , QStringList msg_list   //説明の文言
                               , QSettings *settings
                               , QWidget *parent = 0);
    ~FleetDetailDialog();

    void clear();                           //現状のリストをクリア
    void setParameters(QRect capture_rect         //取り込み範囲
                       , qreal view_ratio         //プレビューの倍率
                       , int columns              //列数
                       , int max                  //取り込み最大数
                       , QStringList msg_list     //説明の文言
                       );
    int initWidth() const;
    void setInitWidth(int initWidth);

signals:
    void finishedCaptureImages(FleetDetailDialog::NextOperationType next
                               , QStringList file_list
                               , int item_width, int item_height
                               , int columns);

private:
    class Private;
    Private *d;

    QSettings *m_settings;
    int m_initWidth;

    void loadSettings();
    void saveSettings();

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
};

#endif // FLEETDETAILDIALOG_H
