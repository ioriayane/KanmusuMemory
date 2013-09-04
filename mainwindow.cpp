﻿/*
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
#include "timerdialog.h"
#include "gamescreen.h"
#include "kanmusumemory_global.h"

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
#include <QSystemTrayIcon>

#include <QScreen>

#include <QtCore/QDebug>

#ifdef Q_OS_MAC
#include <QDesktopWidget>
#endif


#define URL_KANCOLLE "http://www.dmm.com/netgame/social/-/gadgets/=/app_id=854854/"


#define STATUS_BAR_MSG_TIME     5000

class MainWindow::Private
{
public:
    Private(MainWindow *parent);
    ~Private();
    void captureGame();         //保存する
    void checkSavePath();       //保存場所の確認
    void openTweetDialog(const QString &path);     //ツイートダイアログを開く
    void captureCatalog();
    void captureFleetDetail();
    void setFullScreen();

private:
    void maskImage(QImage *img, const QRect &rect);
    QString makeFileName(const QString &format) const;
    void clickGame(QPoint pos, bool wait_little = false);
    MainWindow *q;
    TimerDialog *m_timerDialog;

public:
    Ui::MainWindow ui;
    QSettings settings;         //設定管理
    QSystemTrayIcon trayIcon;   //トレイアイコン
};

MainWindow::Private::Private(MainWindow *parent)
    : q(parent)
    , settings(SETTING_FILE_NAME, SETTING_FILE_FORMAT)
    , trayIcon(QIcon(":/resources/KanmusuMemory32.png"))
{
    ui.setupUi(q);
    ui.webView->page()->networkAccessManager()->setCookieJar(new CookieJar(q));
    QNetworkDiskCache *cache = new QNetworkDiskCache(q);
    cache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    cache->setMaximumCacheSize(1073741824); //about 1024MB
    ui.webView->page()->networkAccessManager()->setCache(cache);

    //通知タイマーのダイアログ作成
    m_timerDialog = new TimerDialog(q, &trayIcon, &settings);

    //メニュー
    connect(ui.capture, &QAction::triggered, [this](){ captureGame(); });
#ifndef DISABLE_CATALOG_AND_DETAIL_FLEET
    connect(ui.captureCatalog, &QAction::triggered, [this](){ captureCatalog(); });
    connect(ui.captureFleetDetail, &QAction::triggered, [this](){ captureFleetDetail(); });
#endif
    connect(ui.reload, &QAction::triggered, ui.webView, &QWebView::reload);
    connect(ui.exit, &QAction::triggered, q, &MainWindow::close);
    connect(ui.actionReturn_to_Kan_Colle, &QAction::triggered, [this]() {
        //艦これ読込み
        ui.webView->load(QUrl(URL_KANCOLLE));
    });
    //画像リスト
    connect(ui.viewMemory, &QAction::triggered, [this]() {
        checkSavePath();
        MemoryDialog dlg(settings.value(QStringLiteral("path")).toString(), q);
        dlg.exec();
        if(QFile::exists(dlg.imagePath()))
            openTweetDialog(dlg.imagePath());
    });
    //通知タイマー
    connect(ui.notificationTimer, &QAction::triggered, [this]() {
        m_timerDialog->show();
    });

    //設定ダイアログ表示
    connect(ui.preferences, &QAction::triggered, [this]() {
        SettingsDialog dlg(q);
        dlg.setSavePath(settings.value(QStringLiteral("path")).toString());
        dlg.setUnusedTwitter(settings.value(SETTING_GENERAL_UNUSED_TWITTER, false).toBool());
        dlg.setSavePng(settings.value(SETTING_GENERAL_SAVE_PNG, false).toBool());
        dlg.setMaskAdmiralName(settings.value(SETTING_GENERAL_MASK_ADMIRAL_NAME, false).toBool());
        dlg.setMaskHqLevel(settings.value(SETTING_GENERAL_MASK_HQ_LEVEL, false).toBool());
        if (dlg.exec()) {
            //設定更新
            settings.setValue(QStringLiteral("path"), dlg.savePath());
            settings.setValue(SETTING_GENERAL_UNUSED_TWITTER, dlg.unusedTwitter());
            settings.setValue(SETTING_GENERAL_SAVE_PNG, dlg.savePng());
            settings.setValue(SETTING_GENERAL_MASK_ADMIRAL_NAME, dlg.isMaskAdmiralName());
            settings.setValue(SETTING_GENERAL_MASK_HQ_LEVEL, dlg.isMaskHqLevel());
        }
    });

    //アバウト
    connect(ui.about, &QAction::triggered, [this]() {
        AboutDialog dlg(q);
        dlg.setVersion(KANMEMO_VERSION);
        dlg.setDevelopers(KANMEMO_DEVELOPERS);
        dlg.exec();
    });

    //フルスクリーン
    q->addAction(ui.actionFullScreen);
    connect(ui.actionFullScreen, &QAction::triggered, [this]() {
        if(q->isFullScreen()){
            //フルスクリーン解除
            q->setWindowState(q->windowState() ^ Qt::WindowFullScreen);
        }else if(ui.webView->gameExists()){
            //フルスクリーンじゃなくてゲームがある
            q->setWindowState(q->windowState() ^ Qt::WindowFullScreen);
        }else{
            //フルスクリーンでゲームがないときは何もしない
        }
    });

    //WebViewの読込み開始
    connect(ui.webView, &QWebView::loadStarted, [this](){
       ui.progressBar->show();
    });
    //WebViewの読込み完了
    connect(ui.webView, &QWebView::loadFinished, [this](bool ok) {
        if (ok) {
            ui.statusBar->showMessage(MainWindow::tr("complete"), STATUS_BAR_MSG_TIME);
//            if(ui.webView->url().toString().compare(URL_KANCOLLE) == 0){
//                setFullScreen();
//            }
        } else {
            ui.statusBar->showMessage(MainWindow::tr("error"), STATUS_BAR_MSG_TIME);
        }
        ui.progressBar->hide();
    });
    //WebViewの読込み状態
    connect(ui.webView, &QWebView::loadProgress, ui.progressBar, &QProgressBar::setValue);

    //通知アイコン
#ifdef Q_OS_WIN
    trayIcon.show();
#endif

#ifdef DISABLE_CATALOG_AND_DETAIL_FLEET
    ui.captureCatalog->setVisible(false);
    ui.captureFleetDetail->setVisible(false);
#endif
}

MainWindow::Private::~Private()
{
    delete m_timerDialog;
}
//指定範囲をマスクする
void MainWindow::Private::maskImage(QImage *img, const QRect &rect)
{
    int y0 = rect.y();
    int x0 = rect.x();
    int h = rect.height();
    int w = rect.width();
    for (int y = 0; y < h; y++) {
        QRgb *row = reinterpret_cast<QRgb *>(img->scanLine(y0 + y)) + x0;
        QRgb pixel = row[0];
        for (int x = 0; x < w; x++) {
            (*row++) = pixel;
        }
    }
}

//ファイル名を作成する
QString MainWindow::Private::makeFileName(const QString &format) const
{
    return QStringLiteral("%1/kanmusu_%2.%3")
            .arg(settings.value(QStringLiteral("path")).toString())
            .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd_hh-mm-ss-zzz")))
            .arg(format.toLower());
}

//思い出を残す
void MainWindow::Private::captureGame()
{
    qDebug() << "captureGame";

    //設定確認
    checkSavePath();

    QImage img = ui.webView->capture();
    if (img.isNull())
    {
        ui.statusBar->showMessage(tr("failed capture image"), STATUS_BAR_MSG_TIME);
        return;
    }

    GameScreen gameScreen(img);

    if (gameScreen.isVisible(GameScreen::HeaderPart)) {
        //提督名をマスク
        if(settings.value(SETTING_GENERAL_MASK_ADMIRAL_NAME, false).toBool()) {
            maskImage(&img, ADMIRAL_RECT_HEADER);
        }
        //司令部レベルをマスク
        if(settings.value(SETTING_GENERAL_MASK_HQ_LEVEL, false).toBool()) {
            maskImage(&img, HQ_LEVEL_RECT_HEADER);
        }
    }

    QString format;
    if(settings.value(SETTING_GENERAL_SAVE_PNG, false).toBool())
        format = QStringLiteral("png");
    else
        format = QStringLiteral("jpg");

    QString path = makeFileName(format);
//    qDebug() << "path:" << path;

    //保存する
    ui.statusBar->showMessage(tr("saving to %1...").arg(path), STATUS_BAR_MSG_TIME);
    if (img.save(path, format.toUtf8().constData())) {
        //つぶやくダイアログ
        openTweetDialog(path);
    } else {
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
    //連携を使用しない
    if(settings.value(SETTING_GENERAL_UNUSED_TWITTER, false).toBool()){
        QMessageBox::information(q
                                 , tr("Kan Memo")
                                 , tr("saving to %1...").arg(path));
    }else{
        //ダイアログを開く
        TweetDialog dlg(q/*
                        , settings.value(SETTING_GENERAL_TOKEN, "").toString()
                        , settings.value(SETTING_GENERAL_TOKENSECRET, "").toString()
                        , settings.value(SETTING_GENERAL_USER_ID, "").toString()
                        , settings.value(SETTING_GENERAL_SCREEN_NAME, "").toString()
                        */);
        dlg.setImagePath(path);
        dlg.setToken(settings.value(SETTING_GENERAL_TOKEN, "").toString());
        dlg.setTokenSecret(settings.value(SETTING_GENERAL_TOKENSECRET, "").toString());
        dlg.user_id(settings.value(SETTING_GENERAL_USER_ID, "").toString());
        dlg.screen_name(settings.value(SETTING_GENERAL_SCREEN_NAME, "").toString());
        dlg.exec();
        settings.setValue(SETTING_GENERAL_TOKEN, dlg.token());
        settings.setValue(SETTING_GENERAL_TOKENSECRET, dlg.tokenSecret());
        settings.setValue(SETTING_GENERAL_USER_ID, dlg.user_id());
        settings.setValue(SETTING_GENERAL_SCREEN_NAME, dlg.screen_name());
    }
}

void MainWindow::Private::clickGame(QPoint pos, bool wait_little)
{
    int interval = CLICK_EVENT_INTERVAL;
    QPointF posf(pos);
    QMouseEvent eventPress(QEvent::MouseButtonPress
                            , posf
                            , Qt::LeftButton, 0, 0);
    QApplication::sendEvent(ui.webView, &eventPress);
    QMouseEvent eventRelease(QEvent::MouseButtonRelease
                              , posf
                              , Qt::LeftButton, 0, 0);
    QApplication::sendEvent(ui.webView, &eventRelease);

    if(wait_little)
        interval = CLICK_EVENT_INTERVAL_LITTLE;

    interval += static_cast<int>(qrand() % CLICK_EVENT_FLUCTUATION);

    for(int i = 0; i < interval; i++)
    {
        QApplication::processEvents();
        QThread::usleep(1000);
    }
}

void MainWindow::Private::captureCatalog()
{
    qDebug() << "captureCatalog";

    //設定確認
    checkSavePath();

    QRect captureRect = CATALOG_RECT_CAPTURE;
    QList<QRect> shipRectList;
    {
        shipRectList << CATALOG_RECT_SHIP1
                     << CATALOG_RECT_SHIP2
                     << CATALOG_RECT_SHIP3;
    }
    QList<QRect> pageRectList;
    {
        pageRectList << CATALOG_RECT_PAGE1
                     << CATALOG_RECT_PAGE2
                     << CATALOG_RECT_PAGE3
                     << CATALOG_RECT_PAGE4
                     << CATALOG_RECT_PAGE5;
    }

    QImage resultImg(captureRect.width() * shipRectList.size()
                     , captureRect.height() * pageRectList.size()
                     ,QImage::Format_ARGB32);
    QPainter painter(&resultImg);

    QPoint currentPos = ui.webView->page()->mainFrame()->scrollPosition();
    QRect geometry = ui.webView->getGameRect();

    if (GameScreen(ui.webView->capture()).screenType() != GameScreen::CatalogScreen)
    {
        ui.webView->page()->mainFrame()->setScrollPosition(currentPos);
        ui.statusBar->showMessage(tr("not in catalog"), STATUS_BAR_MSG_TIME);
        return;
    }

    //開始確認
    QMessageBox::StandardButton res = QMessageBox::warning(q
                                                           , tr("Kan Memo")
                                                           , tr("Capture the catalog.\nPlease wait while a cup of coffee.")
                                                           , QMessageBox::Yes | QMessageBox::Cancel);
    if(res == QMessageBox::Cancel)
        return;

    //メニュー無効
    ui.menuBar->setEnabled(false);
    ui.toolBar->setEnabled(false);

    ui.webView->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui.statusBar->showMessage(tr("making catalog"), -1);
    ui.progressBar->show();
    ui.progressBar->setValue(0);
    for (int type = 0; type < shipRectList.size(); type++)
    {
        int tx = geometry.x()
                + (shipRectList.value(type).x() + qrand() % shipRectList.value(type).width());
        int ty = geometry.y()
                + (shipRectList.value(type).y() + qrand() % shipRectList.value(type).height());
        clickGame(QPoint(tx, ty));

        for (int page = 0; page < pageRectList.size(); page++)
        {
            int px = geometry.x()
                    + (pageRectList.value(page).x() + qrand() % pageRectList.value(page).width());
            int py = geometry.y()
                    + (pageRectList.value(page).y() + qrand() % pageRectList.value(page).height());
            clickGame(QPoint(px, py));

            QImage details = ui.webView->capture().copy(captureRect);
            painter.drawImage(captureRect.width() * type
                              , captureRect.height() * page
                              , details);

            ui.progressBar->setValue((pageRectList.size() * type + (page + 1)) * 100
                                     / (pageRectList.size() * shipRectList.size()) );
        }
    }
    ui.progressBar->hide();
    ui.webView->page()->mainFrame()->setScrollPosition(currentPos);
    ui.webView->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    ui.statusBar->showMessage("", -1);

    char format[] = "jpg";
    QString path = makeFileName(QString(format));
    qDebug() << "path:" << path;

    //保存する
    ui.statusBar->showMessage(tr("saving to %1...").arg(path), STATUS_BAR_MSG_TIME);
    if(resultImg.save(path, format))
    {
        //つぶやくダイアログ
        openTweetDialog(path);
    }else{
        ui.statusBar->showMessage(tr("failed save image"), STATUS_BAR_MSG_TIME);
    }

    //メニュー復活
    ui.menuBar->setEnabled(true);
    ui.toolBar->setEnabled(true);
}

void MainWindow::Private::captureFleetDetail()
{
    qDebug() << "captureFleetDetail";

    //設定確認
    checkSavePath();

    QRect captureRect = DETAIL_RECT_CAPTURE;
    QImage resultImg(captureRect.width() * 2
                     , captureRect.height() * 3
                     ,QImage::Format_ARGB32);
    resultImg.fill(Qt::transparent);
    QPainter painter(&resultImg);

    GameScreen gameScreen(ui.webView->capture());
    if (!gameScreen.isVisible(GameScreen::Ship1Part))
    {
        ui.statusBar->showMessage(tr("not in organization"), STATUS_BAR_MSG_TIME);
        return;
    }

    //開始確認
    QMessageBox::StandardButton res = QMessageBox::warning(q
                                                           , tr("Kan Memo")
                                                           , tr("Capture the fleet ditail.\nPlease wait while a cup of coffee.")
                                                           , QMessageBox::Yes | QMessageBox::Cancel);
    if(res == QMessageBox::Cancel)
        return;

    //メニュー無効
    ui.menuBar->setEnabled(false);
    ui.toolBar->setEnabled(false);

    ui.webView->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui.statusBar->showMessage(tr("making fleet detail"), -1);
    ui.progressBar->show();
    ui.progressBar->setValue(0);
    for (int i = 0; i < 6; i++) {
        if (!gameScreen.isVisible(static_cast<GameScreen::PartType>(GameScreen::Ship1Part + i))) break;
        gameScreen.click(ui.webView, static_cast<GameScreen::PartType>(GameScreen::Ship1Part + i), GameScreen::WaitLonger);

        QImage tmpImg = ui.webView->capture().copy(captureRect);
        painter.drawImage(captureRect.width() * (i % 2)
                          , captureRect.height() * (i / 2)
                          , tmpImg);
        ui.progressBar->setValue((i + 1) * 100 / 6);
        gameScreen.click(ui.webView, GameScreen::Ship1Part);
    }
    ui.progressBar->hide();
    ui.webView->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    ui.statusBar->clearMessage();

    char format[] = "jpg";
    QString path = makeFileName(QString(format));
    qDebug() << "path:" << path;

    //保存する
    ui.statusBar->showMessage(tr("saving to %1...").arg(path), STATUS_BAR_MSG_TIME);
    if(resultImg.save(path, format))
    {
        //つぶやくダイアログ
        openTweetDialog(path);
    }else{
        ui.statusBar->showMessage(tr("failed save image"), STATUS_BAR_MSG_TIME);
    }

    //メニュー復活
    ui.menuBar->setEnabled(true);
    ui.toolBar->setEnabled(true);
}

void MainWindow::Private::setFullScreen()
{
    if(q->isFullScreen()){
        qDebug() << "resize: normal -> full";
        //full
        ui.toolBar->setVisible(false);
        ui.menuBar->setVisible(false);
        ui.statusBar->setVisible(false);

        ui.webView->setViewMode(WebView::FullScreenMode);

    }else{
        qDebug() << "resize: full -> normal";
        //normal
        ui.toolBar->setVisible(true);
        ui.menuBar->setVisible(true);
        ui.statusBar->setVisible(true);

        ui.webView->setViewMode(WebView::NormalMode);
    }

}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d(new Private(this))
{
    //ウインドウの位置を復元
    QSettings settings(SETTING_FILE_NAME, SETTING_FILE_FORMAT);
    settings.beginGroup(QStringLiteral(SETTING_MAINWINDOW));
    restoreGeometry(settings.value(QStringLiteral(SETTING_WINDOW_GEO)).toByteArray());
    restoreState(settings.value(QStringLiteral(SETTING_WINDOW_STATE)).toByteArray());
    settings.endGroup();

    //設定
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    //SSLエラー
    connect(d->ui.webView->page()->networkAccessManager(),
            SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
            this, SLOT(handleSslErrors(QNetworkReply*,QList<QSslError>)));
    //艦これ読込み
    d->ui.webView->load(QUrl(URL_KANCOLLE));

    connect(this, &MainWindow::destroyed, [this]() {delete d;});
}

bool MainWindow::isFullScreen() const
{
#ifdef Q_OS_MAC
    QDesktopWidget desktop;
//    qDebug() << "num:" << desktop.screenNumber(this);
//    qDebug() << "screen geo:" << desktop.screenGeometry(this);
//    qDebug() << "window geo:" << geometry();
    if(geometry().width() == desktop.screenGeometry(this).width()
            && geometry().height() == desktop.screenGeometry(this).height()){
        return true;
    }else{
        return false;
    }
#else
    return QMainWindow::isFullScreen();
#endif
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //フルスクリーン解除
    if(isFullScreen())
        setWindowState(windowState() ^ Qt::WindowFullScreen);

    //ウインドウの位置を保存
    QSettings settings(SETTING_FILE_NAME, SETTING_FILE_FORMAT);
    settings.beginGroup(QStringLiteral(SETTING_MAINWINDOW));
    settings.setValue(QStringLiteral(SETTING_WINDOW_GEO), saveGeometry());
    settings.setValue(QStringLiteral(SETTING_WINDOW_STATE), saveState());
    settings.endGroup();

    QMainWindow::closeEvent(event);
}

void MainWindow::handleSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    QString str = tr("Handle ssl errors.\nDo you want to continue on an understanding of the security risk?\n");
    foreach (QSslError e, errors){
        str.append("ssl error: " + e.errorString());
    }
    qDebug() << str;
    QMessageBox::StandardButton res = QMessageBox::warning(this, "SSL Error", str
                                                               , QMessageBox::Yes | QMessageBox::No);
    if(res == QMessageBox::Yes)
        reply->ignoreSslErrors();
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    static bool prev = false;

    if(prev != isFullScreen()){
        d->setFullScreen();
    }
    prev = isFullScreen();
}
