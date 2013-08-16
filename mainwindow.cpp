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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tweetdialog.h"
#include "settingsdialog.h"
#include "cookiejar.h"
#include "aboutdialog.h"
#include "memorydialog.h"

#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtNetwork/QNetworkDiskCache>
#include <QtWidgets/QMessageBox>
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKit/QWebElement>

#include <QtCore/QDebug>

#define KANMEMO_VERSION "0.3"
#define KANMEMO_DEVELOPERS (QStringList() \
    << "@IoriAYANE"\
    << "@task_jp"\
    )

#define URL_KANCOLLE "http://www.dmm.com/netgame/social/-/gadgets/=/app_id=854854/"

#define SETTING_FILE_NAME       "settings.ini"
#define SETTING_FILE_FORMAT     QSettings::IniFormat

#define STATUS_BAR_MSG_TIME     5000

class MainWindow::Private
{
public:
    Private(MainWindow *parent);
    void captureGame();         //保存する
    void checkSavePath();       //保存場所の確認
    void openTweetDialog(const QString &path);     //ツイートダイアログを開く

private:
    MainWindow *q;
    QtQuick2ApplicationViewer *m_viewer;

public:
    Ui::MainWindow ui;
    QSettings settings;       //設定管理
};

MainWindow::Private::Private(MainWindow *parent)
    : q(parent)
    , settings(SETTING_FILE_NAME, SETTING_FILE_FORMAT)
{
    ui.setupUi(q);
    ui.webView->page()->networkAccessManager()->setCookieJar(new CookieJar(q));
    QNetworkDiskCache *cache = new QNetworkDiskCache(q);
    cache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    ui.webView->page()->networkAccessManager()->setCache(cache);

    connect(ui.capture, &QAction::triggered, [this](){ captureGame(); });
    connect(ui.reload, &QAction::triggered, ui.webView, &QWebView::reload);
    connect(ui.exit, &QAction::triggered, q, &MainWindow::close);
    //画像リスト
    connect(ui.viewMemory, &QAction::triggered, [this]() {
        checkSavePath();
        MemoryDialog dlg(q);
        dlg.setMemoryPath(settings.value(QStringLiteral("path")).toString());
        dlg.setQmlSource(QUrl("qrc:///qml/KanmusuMemory/memoryDialog.qml"));
        dlg.exec();
        if(QFile::exists(dlg.imagePath()))
            openTweetDialog(dlg.imagePath());
    });

    //設定ダイアログ表示
    connect(ui.preferences, &QAction::triggered, [this]() {
        SettingsDialog dlg(q);
        dlg.setSavePath(settings.value(QStringLiteral("path")).toString());
        if (dlg.exec()) {
            //設定更新
            settings.setValue(QStringLiteral("path"), dlg.savePath());
        }
    });

    //アバウト
    connect(ui.about, &QAction::triggered, [this]() {
        AboutDialog dlg(q);
        dlg.setVersion(KANMEMO_VERSION);
        dlg.setDevelopers(KANMEMO_DEVELOPERS);
        dlg.exec();
    });
    //WebViewの読込み開始
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
}

//思い出を残す
void MainWindow::Private::captureGame()
{
    qDebug() << "captureGame";

    //設定確認
    checkSavePath();

    //表示位置を一番上へ強制移動
    QPoint currentPos = ui.webView->page()->mainFrame()->scrollPosition();
    ui.webView->page()->mainFrame()->setScrollPosition(QPoint(0, 0));
    //フレームを取得
    QWebFrame *frame = ui.webView->page()->mainFrame();
    if (frame->childFrames().isEmpty()) {
        ui.webView->page()->mainFrame()->setScrollPosition(currentPos);
        ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return;
    }
    //フレームの子供からflashの入ったdivを探して、さらにその中のembedタグを調べる
    frame = frame->childFrames().first();
    QWebElement element = frame->findFirstElement(QStringLiteral("#flashWrap"));
    if (element.isNull()) {
        ui.webView->page()->mainFrame()->setScrollPosition(currentPos);
        ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return;
    }
    element = element.findFirst(QStringLiteral("embed"));
    if (element.isNull()) {
        ui.webView->page()->mainFrame()->setScrollPosition(currentPos);
        ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return;
    }
    //見つけたタグの座標を取得
    QRect geometry = element.geometry();
    geometry.moveTopLeft(geometry.topLeft() + frame->geometry().topLeft());
    qDebug() << geometry;

    QImage img(geometry.size(), QImage::Format_ARGB32);
    QPainter painter(&img);
    //全体を描画
    ui.webView->render(&painter, QPoint(0,0), geometry);

    //スクロールの位置を戻す
    ui.webView->page()->mainFrame()->setScrollPosition(currentPos);

    QString path = QStringLiteral("%1/kanmusu_%2.png").arg(settings.value(QStringLiteral("path")).toString()).arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd_hh-mm-ss-zzz")));
    qDebug() << "path:" << path;

    //保存する
    ui.statusBar->showMessage(tr("saving to %1...").arg(path), STATUS_BAR_MSG_TIME);
    if(img.save(path)){
        //つぶやくダイアログ
        openTweetDialog(path);
    }else{
        ui.statusBar->showMessage(tr("failed save image"), STATUS_BAR_MSG_TIME);
    }
}

//保存場所の確認
void MainWindow::Private::checkSavePath()
{
    if(!settings.contains(QStringLiteral("path"))){
        //設定を促す
        QMessageBox::information(q
                                 , tr("Kan Memo")
                                 , tr("Please select a folder to save the image of KanMusu."));
        QString savePath = SettingsDialog::selectSavePath(q, QDir::homePath());
        if (savePath.isEmpty()) {
            ui.statusBar->showMessage(tr("canceled"), STATUS_BAR_MSG_TIME);
            return;
        }
        settings.setValue(QStringLiteral("path"), savePath);
    }
}

//ツイートダイアログを開く
void MainWindow::Private::openTweetDialog(const QString &path)
{
    TweetDialog dlg(q);
    dlg.setImagePath(path);
    dlg.setToken(settings.value("token", "").toString());
    dlg.setTokenSecret(settings.value("tokenSecret", "").toString());
    dlg.user_id(settings.value("user_id", "").toString());
    dlg.screen_name(settings.value("screen_name", "").toString());
    dlg.exec();
    settings.setValue("token", dlg.token());
    settings.setValue("tokenSecret", dlg.tokenSecret());
    settings.setValue("user_id", dlg.user_id());
    settings.setValue("screen_name", dlg.screen_name());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d(new Private(this))
{
    //設定
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    //艦これ読込み
    d->ui.webView->load(QUrl(URL_KANCOLLE));

    connect(this, &MainWindow::destroyed, [this]() {delete d;});
}
