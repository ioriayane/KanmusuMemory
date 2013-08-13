/*
 * Copyright 2013 IoriAYANE
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    virtual void resizeEvent(QResizeEvent *);
    virtual void keyPressEvent(QKeyEvent * ev);
private slots:
    void on_capture_triggered();
    void on_reload_triggered();
    void on_exit_triggered();
    void on_adjust_triggered();
    void on_preferences_triggered();
    void on_about_triggered();

public slots:
    //WebViewの読込み状況
    void loadFinished(bool ok);
    void loadProgress(int progress);
    //スクロール状況
    void scrollRequested(int dx, int dy, const QRect &rectToScroll);

signals:
    //ステータスバーへメッセージ表示
    void showMessage(const QString &message, int timeout = 0);

private:
    Ui::MainWindow *ui;

    QSettings m_settings;       //設定管理

    //一時データ
    QPoint m_scroolPos;         //スクロール量
    bool m_finishCalibrated;    //キャリブレーション済み
    //設定保存データ
    QString m_savePath;         //保存パス
    QPoint m_flashPos;          //座標

    void captureGame();         //保存する
    void calibration();         //flashの位置を調べる

    void loadSettings();        //設定を読み込む
    void saveSettings();        //設定を保存する
};

#endif // MAINWINDOW_H
