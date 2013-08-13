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
#include "cookiejar.h"

#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtGui/QRgb>
#include <QtWidgets/QMessageBox>


#include <QtCore/QDebug>

#define URL_KANCOLLE "http://www.dmm.com/netgame/social/-/gadgets/=/app_id=854854/"
#define FLASH_POS_SEARCH_START_Y    40      //Flashの位置を探すときにY方向の開始座標（つまりDMMのヘッダを飛ばす）
#define KANCOLLE_WIDTH   800
#define KANCOLLE_HEIGHT  480

#define SETTING_FILE_NAME       "settings.ini"
#define SETTING_FILE_FORMAT     QSettings::IniFormat

#define STATUS_BAR_MSG_TIME     5000

class MainWindow::Private
{
public:
    Private(MainWindow *parent);
    ~Private();

private:
    MainWindow *q;

public:
    Ui::MainWindow ui;
    QSettings settings;       //設定管理
    //設定保存データ
    QString savePath;         //保存パス
    QPoint flashPos;          //座標

    //一時データ
    QPoint scroolPos;         //スクロール量
    bool finishCalibrated;    //キャリブレーション済み
};

MainWindow::Private::Private(MainWindow *parent)
    : q(parent)
    , settings(SETTING_FILE_NAME, SETTING_FILE_FORMAT)
    , savePath(settings.value("path", "").toString())
    , flashPos(settings.value("flashPos", QPoint(0, 0)).toPoint())
    , finishCalibrated(settings.value("finishCalibrated", false).toBool())
{
    ui.setupUi(q);
    ui.webView->page()->networkAccessManager()->setCookieJar(new CookieJar(q));
    connect(ui.capture, &QAction::triggered, q, &MainWindow::captureGame);
    connect(ui.reload, &QAction::triggered, ui.webView, &QWebView::reload);
    connect(ui.exit, &QAction::triggered, q, &MainWindow::close);
    connect(ui.adjust, &QAction::triggered, q, &MainWindow::calibration);

    //設定ダイアログ表示
    connect(ui.preferences, &QAction::triggered, [this]() {
        SettingsDialog dlg(q);
        dlg.setSavePath(savePath);
        if (dlg.exec()) {
            //設定更新
            savePath = dlg.savePath();
        }
    });

    //アバウト
    connect(ui.about, &QAction::triggered, [this]() {
        QMessageBox::about(q, MainWindow::tr("Kan Memo"), MainWindow::tr("Kan Memo -KanMusu Memory-\n\nCopyright 2013 IoriAYANE"));
    });

    connect(ui.webView, &QWebView::loadStarted, [this](){
       ui.progressBar->show();
    });
    //WebViewの読込み完了
    connect(ui.webView, &QWebView::loadFinished, [this](bool ok) {
        if (ok) {
            ui.statusBar->showMessage(MainWindow::tr("complete"), STATUS_BAR_MSG_TIME);
        } else {
            ui.statusBar->showMessage(MainWindow::tr("error"), STATUS_BAR_MSG_TIME);
        }
        ui.progressBar->hide();
    });

    //WebViewの読込み状態
    connect(ui.webView, &QWebView::loadProgress, ui.progressBar, &QProgressBar::setValue);

    connect(ui.webView->page(), SIGNAL(scrollRequested(int,int,QRect)), q, SLOT(scrollRequested(int,int,QRect)));
}

MainWindow::Private::~Private()
{
    //設定保存
    settings.setValue("path", savePath);
    settings.setValue("flashPos", flashPos);
    settings.setValue("finishCalibrated", finishCalibrated);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d(new Private(this))
{
    if(d->savePath.length() == 0){
        //設定を促す
        QMessageBox::information(this
                                 , tr("Kan Memo")
                                 , tr("Please select a folder to save the image of KanMusu."));
        d->savePath = SettingsDialog::selectSavePath(this, QDir::homePath());
    }

    //設定
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    //艦これ読込み
    d->ui.webView->load(QUrl(URL_KANCOLLE));

    connect(this, &MainWindow::destroyed, [this]() {delete d;});
}

//思い出を残す
void MainWindow::captureGame()
{
    qDebug() << "captureGame";

    if(!d->finishCalibrated){
        //キャリブレーションされてないので確認して実行
        calibration();
    }

    //キャリブレーションしてなければ実行しない
    if(!d->finishCalibrated){
        d->ui.statusBar->showMessage(QString(tr("cancel")), STATUS_BAR_MSG_TIME);
        return;
    }

    int x = d->flashPos.x() + d->scroolPos.x();// int((d->ui.webView->size().width()-16) / 2) - int(800 / 2);
    int y = d->flashPos.y() + d->scroolPos.y();
    QImage img(d->ui.webView->size(), QImage::Format_ARGB32);
    QImage img2(KANCOLLE_WIDTH, KANCOLLE_HEIGHT, QImage::Format_ARGB32);
    QPainter painter(&img);
    QPainter painter2(&img2);
    //全体を描画
    d->ui.webView->page()->view()->render(&painter);
    //2つ目へ必要な分だけコピー
    painter2.drawImage(QPoint(0,0), img, QRect(x, y, KANCOLLE_WIDTH, KANCOLLE_HEIGHT));
    QDate date(QDate::currentDate());
    QTime time(QTime::currentTime());
    QString path = d->savePath + "/kanmusu";
    path.append(QString("_%1-%2-%3_%4-%5-%6-%7")
                .arg(date.year()).arg(date.month()).arg(date.day())
                .arg(time.hour()).arg(time.minute()).arg(time.second()).arg(time.msec())
                );
    path.append(".png");
    //    path.append(".jpg");
    qDebug() << "path:" << path;

    //保存する
    if(img2.save(path)){
        d->ui.statusBar->showMessage(tr("saving to %1...").arg(path), STATUS_BAR_MSG_TIME);


        //つぶやくダイアログ
        TweetDialog dlg(this);
        dlg.setImagePath(path);
        dlg.setToken(d->settings.value("token", "").toString());
        dlg.setTokenSecret(d->settings.value("tokenSecret", "").toString());
        dlg.user_id(d->settings.value("user_id", "").toString());
        dlg.screen_name(d->settings.value("screen_name", "").toString());
        dlg.exec();
        d->settings.setValue("token", dlg.token());
        d->settings.setValue("tokenSecret", dlg.tokenSecret());
        d->settings.setValue("user_id", dlg.user_id());
        d->settings.setValue("screen_name", dlg.screen_name());
        //        d->settings.sync();

    }else{
        d->ui.statusBar->showMessage(tr("failed"), STATUS_BAR_MSG_TIME);
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
    QImage img(d->ui.webView->size(), QImage::Format_ARGB32);
    QPainter painter(&img);
    int w = d->ui.webView->size().width();
    int h = d->ui.webView->size().height();
    QRgb rgb;

    //全体を描画
    d->ui.webView->page()->view()->render(&painter);

    //横方向にはじっこを調べる
    for(int i=0; i<(w/2); i++){
        rgb = img.pixel(i, h/2);
        if(qGray(rgb) < 250){
            qDebug() << "found X:" << i << "," << (h/2) << "/" << qGray(rgb)
                     << "/" << qRed(rgb) << "," << qGreen(rgb) << "," << qBlue(rgb);
            d->flashPos.setX(i);
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
    //            d->flashPos.setY(i - KANKORE_HEIGHT + 1 - d->scroolPos.y());
    //            set_count++;
    //            break;
    //        }
    //    }
    for(int i=FLASH_POS_SEARCH_START_Y; i<h; i++){
        rgb = img.pixel(w/2, i);
        if(qGray(rgb) < 250){
            qDebug() << "found Y:" << (w/2) << "," << i << "/" << qGray(rgb)
                     << "/" << qRed(rgb) << "," << qGreen(rgb) << "," << qBlue(rgb);
            d->flashPos.setY(i - d->scroolPos.y());
            set_count++;
            break;
        }
    }
    //キャリブレーション済み
    if(set_count == 2){
        d->ui.statusBar->showMessage(tr("calibration succeded"), STATUS_BAR_MSG_TIME);
        d->finishCalibrated = true;
    }else{
        d->ui.statusBar->showMessage(tr("calibration failed"), STATUS_BAR_MSG_TIME);
        d->finishCalibrated = false;
    }
}

//スクロール状態
void MainWindow::scrollRequested(int dx, int dy, const QRect &rectToScroll)
{
    Q_UNUSED(dx)
    Q_UNUSED(rectToScroll)
    //    qDebug() << "scroll:" << dx << "," << dy << "/" << rectToScroll;
    d->scroolPos.setY(d->scroolPos.y() + dy);
    //    if(d->scroolPos.y() < 0)
    //        d->scroolPos.setY(0);

    qDebug() << "scroll:" << d->scroolPos.y();
}
