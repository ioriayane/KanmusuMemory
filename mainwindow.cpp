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

#include <QtCore/QDebug>


#define URL_KANCOLLE "http://www.dmm.com/netgame/social/-/gadgets/=/app_id=854854/"


#define STATUS_BAR_MSG_TIME     5000
#define CLICK_EVENT_INTERVAL            1500
#define CLICK_EVENT_INTERVAL_LITTLE     500

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

private:
    QRect getGameRect();
    QImage getGameImage(QRect crop = QRect());
    QString makeFileName(const QString &format) const;
    void clickGame(QPoint pos, bool wait_little = false);
    bool isCatalogScreen();
    bool isShipExist(QRect rect1, QRect rect2);
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
    connect(ui.captureCatalog, &QAction::triggered, [this](){ captureCatalog(); });
    connect(ui.captureFleetDetail, &QAction::triggered, [this](){ captureFleetDetail(); });
    connect(ui.reload, &QAction::triggered, ui.webView, &QWebView::reload);
    connect(ui.exit, &QAction::triggered, q, &MainWindow::close);
    //画像リスト
    connect(ui.viewMemory, &QAction::triggered, [this]() {
        checkSavePath();
        MemoryDialog dlg(q);
        dlg.setMemoryPath(settings.value(QStringLiteral("path")).toString());
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
        if (dlg.exec()) {
            //設定更新
            settings.setValue(QStringLiteral("path"), dlg.savePath());
            settings.setValue(SETTING_GENERAL_UNUSED_TWITTER, dlg.unusedTwitter());
            settings.setValue(SETTING_GENERAL_SAVE_PNG, dlg.savePng());
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
    //通知アイコン
#ifdef Q_OS_WIN
    trayIcon.show();
#endif
}

MainWindow::Private::~Private()
{
    delete m_timerDialog;
}

QRect MainWindow::Private::getGameRect()
{
    //スクロール位置は破壊される
    //表示位置を一番上へ強制移動
    ui.webView->page()->mainFrame()->setScrollPosition(QPoint(0, 0));
    //フレームを取得
    QWebFrame *frame = ui.webView->page()->mainFrame();
    if (frame->childFrames().isEmpty()) {
        ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return QRect();
    }
    //フレームの子供からflashの入ったdivを探して、さらにその中のembedタグを調べる
    frame = frame->childFrames().first();
    QWebElement element = frame->findFirstElement(QStringLiteral("#flashWrap"));
    if (element.isNull()) {
        ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return QRect();
    }
    element = element.findFirst(QStringLiteral("embed"));
    if (element.isNull()) {
        ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return QRect();
    }
    //見つけたタグの座標を取得
    QRect geometry = element.geometry();
    geometry.moveTopLeft(geometry.topLeft() + frame->geometry().topLeft());

    return geometry;
}

QImage MainWindow::Private::getGameImage(QRect crop)
{
    //スクロール位置の保存
    QPoint currentPos = ui.webView->page()->mainFrame()->scrollPosition();
    QRect geometry = getGameRect();
    qDebug() << geometry;
    if (!geometry.isValid())
    {
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

    char format[4] = {0};
    if(settings.value(SETTING_GENERAL_SAVE_PNG, false).toBool())
        strcpy(format, "png");
    else
        strcpy(format, "jpg");

    QString path = makeFileName(QString(format));
    qDebug() << "path:" << path;

    //保存する
    ui.statusBar->showMessage(tr("saving to %1...").arg(path), STATUS_BAR_MSG_TIME);
    if(img.save(path, format)){
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
    //連携を使用しない
    if(settings.value(SETTING_GENERAL_UNUSED_TWITTER, false).toBool()){
        QMessageBox::information(q
                                 , tr("Kan Memo")
                                 , tr("saving to %1...").arg(path));
    }else{
        //ダイアログを開く
        TweetDialog dlg(q);
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

    for(int i = 0; i < interval; i++)
    {
        QApplication::processEvents();
        QThread::usleep(1000);
    }
}

bool MainWindow::Private::isCatalogScreen()
{
    QImage img = getGameImage().copy(CATALOG_CHECK_RECT);
    int r = 0;
    int g = 0;
    int b = 0;
    for(int y = 0; y < img.height(); y++)
        for (int x = 0; x < img.width(); x++)
        {
            r += qRed(img.pixel(x, y));
            g += qGreen(img.pixel(x, y));
            b += qBlue(img.pixel(x, y));
        }
    r /= (img.width() * img.height());
    g /= (img.width() * img.height());
    b /= (img.width() * img.height());

    qDebug() << "check:" << r << g << b;
    QRgb chkRgb = CATALOG_CHECK_COLOR;
    if (r < qRed(chkRgb) && g < qGreen(chkRgb) && b < qBlue(chkRgb)
     && qRed(chkRgb) - 10 < r && qGreen(chkRgb) -10 < g && qBlue(chkRgb) -10 < b)
        return true;
    else
        return false;
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
    QRect geometry = getGameRect();

    if (!isCatalogScreen())
    {
        ui.webView->page()->mainFrame()->setScrollPosition(currentPos);
        ui.statusBar->showMessage(tr("not in catalog"), STATUS_BAR_MSG_TIME);
        return;
    }

    ui.webView->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui.statusBar->showMessage(tr("making catalog"), -1);
    ui.progressBar->show();
    ui.progressBar->setValue(0);
    for (int type = 0; type < shipRectList.size(); type++)
    {
        int tx = geometry.x()
                + (shipRectList.value(type).x() + shipRectList.value(type).width() / 2);
        int ty = geometry.y()
                + (shipRectList.value(type).y() + shipRectList.value(type).height() / 2);
        clickGame(QPoint(tx, ty));

        for (int page = 0; page < pageRectList.size(); page++)
        {
            int px = geometry.x()
                    + (pageRectList.value(page).x() + pageRectList.value(page).width() / 2);
            int py = geometry.y()
                    + (pageRectList.value(page).y() + pageRectList.value(page).height() / 2);
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
    QRect geometry = getGameRect();

    if (!isShipExist(shipRectList.value(0), checkRectList.value(0)))
    {
        ui.webView->page()->mainFrame()->setScrollPosition(currentPos);
        ui.statusBar->showMessage(tr("not in organization"), STATUS_BAR_MSG_TIME);
        return;
    }

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
                + (shipRectList.value(i).x() + shipRectList.value(i).width() / 2);
        int py = geometry.y()
                + (shipRectList.value(i).y() + shipRectList.value(i).height() / 2);
        //open
        clickGame(QPoint(px, py));
        tmpImg = getGameImage(captureRect);
        painter.drawImage(captureRect.width() * (i % 2)
                          , captureRect.height() * (i / 2)
                          , tmpImg);
        ui.progressBar->setValue((i + 1) * 100 / shipRectList.size());
        //close
        clickGame(QPoint(geometry.x() + 370, geometry.y() + 120), true);
    }
    ui.progressBar->hide();
    ui.webView->page()->mainFrame()->setScrollPosition(currentPos);
    ui.webView->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    ui.statusBar->showMessage("", -1);

    char format[] = "png";
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

void MainWindow::closeEvent(QCloseEvent *event)
{
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
