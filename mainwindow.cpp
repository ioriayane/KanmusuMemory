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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tweetdialog.h"
#include "settingsdialog.h"

#include <QDate>
#include <QTime>
#include <QString>
#include <QDir>
#include <QRgb>
#include <QMessageBox>


#include <QDebug>

#define URL_KANCOLLE "http://www.dmm.com/netgame/social/-/gadgets/=/app_id=854854/"
#define FLASH_POS_SEARCH_START_Y    40      //Flashの位置を探すときにY方向の開始座標（つまりDMMのヘッダを飛ばす）
#define KANCOLLE_WIDTH   800
#define KANCOLLE_HEIGHT  480

#define SETTING_FILE_NAME       "settings.ini"
#define SETTING_FILE_FORMAT     QSettings::IniFormat

#define STATUS_BAR_MSG_TIME     5000


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_savePath(""),
    m_finishCalibrated(false),
    m_settings(SETTING_FILE_NAME, SETTING_FILE_FORMAT, this)
{
    ui->setupUi(this);
    centralWidget()->deleteLater();
    setCentralWidget(ui->webView);

    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    connect(ui->webView, SIGNAL(loadProgress(int)), this, SLOT(loadProgress(int)));
    connect(ui->webView->page(), SIGNAL(scrollRequested(int,int,QRect)), this, SLOT(scrollRequested(int,int,QRect)));

    connect(this, SIGNAL(showMessage(QString)), ui->statusBar, SLOT(showMessage(QString)));
    connect(this, SIGNAL(showMessage(QString, int)), ui->statusBar, SLOT(showMessage(QString, int)));

    //設定読込み
    loadSettings();

    if(m_savePath.length() == 0){
        //設定を促す
        QMessageBox::information(this
                                 , tr("Kan Memo")
                                 , tr("Please select a folder to save the image of KanMusu."));
        m_savePath = SettingsDialog::selectSavePath(this, QDir::homePath());
    }

    //設定
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    //艦これ読込み
    ui->webView->load(QUrl(URL_KANCOLLE));

}

MainWindow::~MainWindow()
{
    //設定保存
    saveSettings();

    delete ui;
}



void MainWindow::resizeEvent(QResizeEvent *)
{
    //#error "これが起きてキャリブレーション済みの設定が消える.からまぁなしで"
    //サイズが変わったらキャリブレーション解除
    //    m_finishCalibrated = false;
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    //F9押したらキャプチャ
    if(ev->key() == Qt::Key_F9)
        captureGame();
}

//思い出を残す（ボタン）
void MainWindow::on_capture_triggered()
{
    captureGame();
}
//思い出を残す
void MainWindow::captureGame()
{
    qDebug() << "captureGame";

    if(!m_finishCalibrated){
        //キャリブレーションされてないので確認して実行
        calibration();
    }

    //キャリブレーションしてなければ実行しない
    if(!m_finishCalibrated){
        emit showMessage(QString(tr("cancel")), STATUS_BAR_MSG_TIME);
        return;
    }

    int x = m_flashPos.x() + m_scroolPos.x();// int((ui->webView->size().width()-16) / 2) - int(800 / 2);
    int y = m_flashPos.y() + m_scroolPos.y();
    QImage img(ui->webView->size(), QImage::Format_ARGB32);
    QImage img2(KANCOLLE_WIDTH, KANCOLLE_HEIGHT, QImage::Format_ARGB32);
    QPainter painter(&img);
    QPainter painter2(&img2);
    //全体を描画
    ui->webView->page()->view()->render(&painter);
    //2つ目へ必要な分だけコピー
    painter2.drawImage(QPoint(0,0), img, QRect(x, y, KANCOLLE_WIDTH, KANCOLLE_HEIGHT));
    QDate date(QDate::currentDate());
    QTime time(QTime::currentTime());
    QString path = m_savePath + "/kanmusu";
    path.append(QString("_%1-%2-%3_%4-%5-%6-%7")
                .arg(date.year()).arg(date.month()).arg(date.day())
                .arg(time.hour()).arg(time.minute()).arg(time.second()).arg(time.msec())
                );
    path.append(".png");
    //    path.append(".jpg");
    qDebug() << "path:" << path;

    //保存する
    if(img2.save(path)){
        emit showMessage(QString(tr("save...")) + path, STATUS_BAR_MSG_TIME);


        //つぶやくダイアログ
        TweetDialog dlg(this);
        dlg.setImagePath(path);
        dlg.setToken(m_settings.value("token", "").toString());
        dlg.setTokenSecret(m_settings.value("tokenSecret", "").toString());
        dlg.user_id(m_settings.value("user_id", "").toString());
        dlg.screen_name(m_settings.value("screen_name", "").toString());
        dlg.exec();
        m_settings.setValue("token", dlg.token());
        m_settings.setValue("tokenSecret", dlg.tokenSecret());
        m_settings.setValue("user_id", dlg.user_id());
        m_settings.setValue("screen_name", dlg.screen_name());
        //        m_settings.sync();

    }else{
        emit showMessage(QString(tr("fail")), STATUS_BAR_MSG_TIME);
    }

}
//キャリブレーション
void MainWindow::calibration()
{
    qDebug() << "calibration";

//    QMessageBox::information(this
//                             , tr("Information")
//                             , tr("Calibrate Kankore position."));
    QMessageBox msgdlg(this);
    msgdlg.setWindowTitle(tr("Information"));
    msgdlg.setText(tr("Search Kankore position.\nPlease be performed in the state that it seems the whole of KanColle."));
    msgdlg.setStandardButtons(QMessageBox::Yes);
    msgdlg.addButton("No", QMessageBox::RejectRole);
    msgdlg.exec();
    qDebug() << "calib:" << msgdlg.result();
    if(msgdlg.result() != QMessageBox::Yes)
        return;

    int set_count = 0;
    QImage img(ui->webView->size(), QImage::Format_ARGB32);
    QPainter painter(&img);
    int w = ui->webView->size().width();
    int h = ui->webView->size().height();
    QRgb rgb;

    //全体を描画
    ui->webView->page()->view()->render(&painter);

    //横方向にはじっこを調べる
    for(int i=0; i<(w/2); i++){
        rgb = img.pixel(i, h/2);
        if(qGray(rgb) < 250){
            qDebug() << "found X:" << i << "," << (h/2) << "/" << qGray(rgb)
                     << "/" << qRed(rgb) << "," << qGreen(rgb) << "," << qBlue(rgb);
            m_flashPos.setX(i);
            set_count++;
            break;
        }
    }
    //縦方向に端っこを調べる
    //    for(int i=h-1; i>=(h/2); i--){
    //        rgb = img.pixel(w/2, i);
    //        if(qGray(rgb) < 250){
    //            qDebug() << "found Y:" << (w/2) << "," << (i-KANKORE_HEIGHT) << "/" << qGray(rgb)
    //                     << "/" << qRed(rgb) << "," << qGreen(rgb) << "," << qBlue(rgb);
    //            m_flashPos.setY(i - KANKORE_HEIGHT + 1 - m_scroolPos.y());
    //            set_count++;
    //            break;
    //        }
    //    }
    for(int i=FLASH_POS_SEARCH_START_Y; i<h; i++){
        rgb = img.pixel(w/2, i);
        if(qGray(rgb) < 250){
            qDebug() << "found Y:" << (w/2) << "," << i << "/" << qGray(rgb)
                     << "/" << qRed(rgb) << "," << qGreen(rgb) << "," << qBlue(rgb);
            m_flashPos.setY(i - m_scroolPos.y());
            set_count++;
            break;
        }
    }
    //キャリブレーション済み
    if(set_count == 2){
        emit showMessage(QString(tr("success calibration")), STATUS_BAR_MSG_TIME);
        m_finishCalibrated = true;
    }else{
        emit showMessage(QString(tr("fail calibration")), STATUS_BAR_MSG_TIME);
        m_finishCalibrated = false;
    }
}

//設定を読込み
void MainWindow::loadSettings()
{
    m_savePath = m_settings.value("path", "").toString();
    m_flashPos = m_settings.value("flashPos", QPoint(0, 0)).toPoint();
    m_finishCalibrated = m_settings.value("finishCalibrated", false).toBool();

}
//設定を保存
void MainWindow::saveSettings()
{
    m_settings.setValue("path", m_savePath);
    m_settings.setValue("flashPos", m_flashPos);
    m_settings.setValue("finishCalibrated", m_finishCalibrated);
    //    m_settings.sync();
    qDebug() << "save settings";
}

//再読み込み
void MainWindow::on_reload_triggered()
{
    qDebug() << "reload";
    ui->webView->load(QUrl(URL_KANCOLLE));
}
//終了
void MainWindow::on_exit_triggered()
{
    qDebug() << "exit";
    close();
}
//Flashの位置を探す
void MainWindow::on_adjust_triggered()
{
    calibration();
}

//WebViewの読込み完了
void MainWindow::loadFinished(bool ok)
{
    if(ok)
        emit showMessage(QString(tr("complete")));
    else
        emit showMessage(QString(tr("error")));
}
//WebViewの読込み状態
void MainWindow::loadProgress(int progress)
{
    emit showMessage(QString(tr("loading...%1%")).arg(progress));
}
//スクロール状態
void MainWindow::scrollRequested(int dx, int dy, const QRect &rectToScroll)
{
    //    qDebug() << "scroll:" << dx << "," << dy << "/" << rectToScroll;
    m_scroolPos.setY(m_scroolPos.y() + dy);
    //    if(m_scroolPos.y() < 0)
    //        m_scroolPos.setY(0);

    qDebug() << "scroll:" << m_scroolPos.y();
}

//設定ダイアログ表示
void MainWindow::on_preferences_triggered()
{
    SettingsDialog dlg(this);
    dlg.setSavePath(m_savePath);
    dlg.exec();
    if(dlg.result() != 0){
        //設定更新
        m_savePath = dlg.savePath();
    }
}
//アバウト
void MainWindow::on_about_triggered()
{
    QMessageBox::about(this, tr("Kan Memo"), tr("Kan Memo -KanMusu Memory-\n\nCopyright 2013 IoriAYANE"));
}
