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
#include "timerdialog.h"
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


#define URL_KANCOLLE "http://www.dmm.com/netgame/social/-/gadgets/=/app_id=854854/"


#define STATUS_BAR_MSG_TIME     5000
#define CLICK_EVENT_INTERVAL            1500
#define CLICK_EVENT_INTERVAL_LITTLE     500
#define CLICK_EVENT_FLUCTUATION         500

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
    QImage getGameImage(const QRect &crop = QRect());
    void maskImage(QImage *img, const QRect &rect);
    QString makeFileName(const QString &format) const;
    void clickGame(QPoint pos, bool wait_little = false);
    bool isDesiredScreen(const QImage &image, const QRect &rect, const QRgb &rgb) const;
    bool isCatalogScreen();
    bool isShipExist(QRect rect1, QRect rect2);
    MainWindow *q;
    TimerDialog *m_timerDialog;
    WebPageOperation m_webOpe;

public:
    Ui::MainWindow ui;
    QSettings settings;         //設定管理
    QSystemTrayIcon trayIcon;   //トレイアイコン
};

MainWindow::Private::Private(MainWindow *parent)
    : q(parent)
    , settings(SETTING_FILE_NAME, SETTING_FILE_FORMAT)
    , trayIcon(QIcon(":/resources/KanmusuMemory32.png"))
    , m_webOpe(parent)
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
        }else if(m_webOpe.existGame()){
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
    //WebPageの解析
    m_webOpe.setWebView(ui.webView);
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
//ゲーム画面をキャプチャ
QImage MainWindow::Private::getGameImage(const QRect &crop)
{
    //スクロール位置の保存
    QPoint currentPos = ui.webView->page()->mainFrame()->scrollPosition();
    QRect geometry = m_webOpe.getGameRect();
//    qDebug() << geometry;
    if (!geometry.isValid())
    {
        ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        ui.webView->page()->mainFrame()->setScrollPosition(currentPos);
        return QImage();
    }

    QImage img(geometry.size(), QImage::Format_ARGB32);
    QPainter painter(&img);
    //全体を描画
    ui.webView->render(&painter, QPoint(0,0), geometry);

    //スクロールの位置を戻す
    ui.webView->page()->mainFrame()->setScrollPosition(currentPos);

    return img.copy(crop);
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

    QImage img = getGameImage();
    if (img.isNull())
    {
        ui.statusBar->showMessage(tr("failed capture image"), STATUS_BAR_MSG_TIME);
        return;
    }

    //提督名をマスク
    if(settings.value(SETTING_GENERAL_MASK_ADMIRAL_NAME, false).toBool()
            && isDesiredScreen(img, HOME_PORT_RECT_CAPTURE, HOME_PORT_CHECK_COLOR))
    {
        maskImage(&img, ADMIRAL_RECT_HEADER);
    }
    //司令部レベルをマスク
    if(settings.value(SETTING_GENERAL_MASK_HQ_LEVEL, false).toBool()
            && isDesiredScreen(img, HOME_PORT_RECT_CAPTURE, HOME_PORT_CHECK_COLOR))
    {
        maskImage(&img, HQ_LEVEL_RECT_HEADER);
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

//目的の画面か含まれる色の成分で調べる
bool MainWindow::Private::isDesiredScreen(const QImage &image, const QRect &rect, const QRgb &rgb) const
{
    QRgb pixel = image.copy(rect).scaled(1, 1).pixel(0, 0);
    return qAbs(qRed(pixel) - qRed(rgb)) < 0x20 && qAbs(qGreen(pixel) - qGreen(rgb)) < 0x20 && qAbs(qBlue(pixel) - qBlue(rgb)) < 0x20;
}

//カタログ画面か
bool MainWindow::Private::isCatalogScreen()
{
    return isDesiredScreen(getGameImage(), CATALOG_CHECK_RECT, CATALOG_CHECK_COLOR);
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

    QImage tmpImg(captureRect.size(), QImage::Format_ARGB32);
    QImage resultImg(captureRect.width() * shipRectList.size()
                     , captureRect.height() * pageRectList.size()
                     ,QImage::Format_ARGB32);
    QPainter painter(&resultImg);

    QPoint currentPos = ui.webView->page()->mainFrame()->scrollPosition();
    QRect geometry = m_webOpe.getGameRect();

    if (!isCatalogScreen())
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

            tmpImg = getGameImage(captureRect);
            painter.drawImage(captureRect.width() * type
                              , captureRect.height() * page
                              , tmpImg);

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

bool MainWindow::Private::isShipExist(QRect rect1, QRect rect2)
{
    QImage img = getGameImage();
    int yr = 0;
    int yg = 0;
    int yb = 0;
    for(int y = rect1.y(); y < rect1.y() + rect1.height(); y++)
        for (int x = rect1.x(); x <rect1.x() + rect1.width(); x++)
        {
            yr += qRed(img.pixel(x, y));
            yg += qGreen(img.pixel(x, y));
            yb += qBlue(img.pixel(x, y));
        }
    yr /= (rect1.width() * rect1.height());
    yg /= (rect1.width() * rect1.height());
    yb /= (rect1.width() * rect1.height());

    bool result1 = false;
    QRgb chk1 = DETAIL_CHECK_COLOR1;
    if (yr < qRed(chk1) && yg < qGreen(chk1) && yb < qBlue(chk1)
     && qRed(chk1) - 20 < yr && qGreen(chk1) - 20 < yg && qBlue(chk1) - 20 < yb)
        result1 = true;

    int gr = 0;
    int gg = 0;
    int gb = 0;
    for(int y = rect2.y(); y < rect2.y() + rect2.height(); y++)
        for (int x = rect2.x(); x <rect2.x() + rect2.width(); x++)
        {
            gr += qRed(img.pixel(x, y));
            gg += qGreen(img.pixel(x, y));
            gb += qBlue(img.pixel(x, y));
        }
    gr /= (rect2.width() * rect2.height());
    gg /= (rect2.width() * rect2.height());
    gb /= (rect2.width() * rect2.height());

    qDebug() << "check:" << yr << yg << yb << "|" << gr << gg << gb;

    bool result2 = false;
    QRgb chk2 = DETAIL_CHECK_COLOR2;
    if (gr < qRed(chk2) && gg < qGreen(chk2) && gb < qBlue(chk2)
     && qRed(chk2) - 30 < gr && qGreen(chk2) - 30 < gg && qBlue(chk2) - 30 < gb)
        result2 = true;

    return result1 & result2;
}


void MainWindow::Private::captureFleetDetail()
{
    qDebug() << "captureFleetDetail";

    //設定確認
    checkSavePath();

    QRect captureRect = DETAIL_RECT_CAPTURE;
    QList<QRect> shipRectList;
    {
        shipRectList << DETAIL_RECT_SHIP1
                     << DETAIL_RECT_SHIP2
                     << DETAIL_RECT_SHIP3
                     << DETAIL_RECT_SHIP4
                     << DETAIL_RECT_SHIP5
                     << DETAIL_RECT_SHIP6;
    }
    QList<QRect> checkRectList;
    {
        checkRectList << DETAIL_RECT_SHIP1_2
                     << DETAIL_RECT_SHIP2_2
                     << DETAIL_RECT_SHIP3_2
                     << DETAIL_RECT_SHIP4_2
                     << DETAIL_RECT_SHIP5_2
                     << DETAIL_RECT_SHIP6_2;
    }

    QImage tmpImg(captureRect.size(), QImage::Format_ARGB32);
    QImage resultImg(captureRect.width() * 2
                     , captureRect.height() * (shipRectList.size() / 2 + (shipRectList.size() % 2 == 1))
                     ,QImage::Format_ARGB32);
    QPainter painter(&resultImg);

    QPoint currentPos = ui.webView->page()->mainFrame()->scrollPosition();
    QRect geometry = m_webOpe.getGameRect();

    if (!isShipExist(shipRectList.value(0), checkRectList.value(0)))
    {
        ui.webView->page()->mainFrame()->setScrollPosition(currentPos);
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
    for (int i = 0; i < shipRectList.size(); i++)
    {
        if (!isShipExist(shipRectList.value(i), checkRectList.value(i)))
        {
            QRect box = captureRect;
            box.moveTo(captureRect.width() * (i % 2), captureRect.height() * (i / 2));
            painter.fillRect(box, Qt::SolidPattern);
            continue;
        }
        int px = geometry.x()
                + (shipRectList.value(i).x() + qrand() % shipRectList.value(i).width());
        int py = geometry.y()
                + (shipRectList.value(i).y() + qrand() % shipRectList.value(i).height());
        //open
        clickGame(QPoint(px, py));
        tmpImg = getGameImage(captureRect);
        painter.drawImage(captureRect.width() * (i % 2)
                          , captureRect.height() * (i / 2)
                          , tmpImg);
        ui.progressBar->setValue((i + 1) * 100 / shipRectList.size());
        //close
        px = geometry.x()
                + (DETAIL_RECT_CLOSE.x() + qrand() % DETAIL_RECT_CLOSE.width());
        py = geometry.y()
                + (DETAIL_RECT_CLOSE.y() + qrand() % DETAIL_RECT_CLOSE.height());
        clickGame(QPoint(px, py), true);
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

void MainWindow::Private::setFullScreen()
{
    if(q->isFullScreen()){
        qDebug() << "resize: normal -> full";
        //full
        ui.toolBar->setVisible(false);
        ui.menuBar->setVisible(false);
        ui.statusBar->setVisible(false);

        m_webOpe.fullScreen(true);

    }else{
        qDebug() << "resize: full -> normal";
        //normal
        ui.toolBar->setVisible(true);
        ui.menuBar->setVisible(true);
        ui.statusBar->setVisible(true);

        m_webOpe.fullScreen(false);
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
