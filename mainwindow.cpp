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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tweetdialog.h"
#include "settingsdialog.h"
#include "cookiejar.h"
#include "aboutdialog.h"
#include "memorydialog.h"
#include "imageeditdialog.h"
#include "timerdialog.h"
#include "updateinfodialog.h"
#include "fleetdetaildialog.h"
#include "gamescreen.h"
#include "webpageform.h"
#include "favoritemenu.h"
#include "recordingthread.h"
#include "recordingdialog.h"
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

#define SPLIT_WEBPAGE_INDEX     1       //分割ウインドウの通常ブラウザのインデックス

#define STATUS_BAR_MSG_TIME     5000

class MainWindow::Private
{
public:
    Private(MainWindow *parent, bool not_use_cookie);
    ~Private();
    void captureGame(bool andEdit = false);         //保存する
    void checkSavePath();                           //保存場所の確認
    void openTweetDialog(const QString &path, bool force = false);      //ツイートダイアログを開く
    void openMemoryDialog();
    void openSettingDialog();
    void openRecordSettingDialog();
    void updateProxyConfiguration();
    void openAboutDialog();
    void openImageEditDialog(const QString &path, const QString &tempPath, const QString &editPath);
    void openManualCaptureFleetDetail();
    void openManualCaptureFleetList();
    void finishManualCaptureFleetDetail(FleetDetailDialog::NextOperationType next, QStringList file_list, int item_width, int item_height, int columns);
    void captureCatalog();
    void captureFleetDetail();
    void setFullScreen();
    void setGameSize(qreal factor);
    void checkMajorDamageShip(const QPointF &pos, bool force = false);
    void checkExpeditionRemainTime(const QPointF &pos);

    QList<int> bakSplitterSizes;    //幅のサイズ保存用にとっておく。（非表示だと0になってしまうから）
private:
    void setWebSettings();          //Web関連の設定をする
    void makeDialog();              //ダイアログの作成をする
    void setButtleResultPosition(); //戦果報告の表示位置

    void maskImage(QImage *img, const QRect &rect);
    QString makeFileName(const QString &format, bool isfull = true) const;
    QString makeTempFileName(const QString &format) const;
    void clickGame(QPoint pos, bool wait_little = false);
    QString combineImage(QStringList file_list, int item_width, int item_height, int columns);

    void setSplitWindowVisiblity(bool visible);
    bool isSplitWindowVisible();
    void setSplitWindowOrientation(Qt::Orientation orientation);

    MainWindow *q;
    TimerDialog *m_timerDialog;
    UpdateInfoDialog *m_updateInfoDialog;
    FleetDetailDialog *m_fleetDetailDialog;
    FavoriteMenu m_favorite;
    RecordingThread recordingThread;
    bool dontViewButtleResult;          //録画中は戦績表示しない
    bool muteTimerSound;                //録画中はミュート

public:
    Ui::MainWindow ui;
    QSettings settings;         //設定管理
    QSystemTrayIcon trayIcon;   //トレイアイコン
    bool notUseCookie;

public slots:
    void clickItem(){}

};

MainWindow::Private::Private(MainWindow *parent, bool not_use_cookie)
    : q(parent)
    , settings(QSettings::IniFormat, QSettings::UserScope, KANMEMO_PROJECT, KANMEMO_NAME)
    , trayIcon(QIcon(":/resources/KanmusuMemory32.png"))
    , m_favorite(q)
    , notUseCookie(not_use_cookie)
{
    ui.setupUi(q);

    //Web関連の設定
    setWebSettings();
    //ダイアログの作成
    makeDialog();

    //戦績報告を非表示
    ui.viewButtleResult->setVisible(false);
    setButtleResultPosition();

    ///////////////////////////////////////////////////////////////
    /// メニュー
    ///////////////////////////////////////////////////////////////
    connect(ui.capture, &QAction::triggered, [this](){ captureGame(); });
    connect(ui.actionCaptureAndEdit, &QAction::triggered, [this]() { captureGame(true); });
#ifndef DISABLE_CATALOG_AND_DETAIL_FLEET
    connect(ui.captureCatalog, &QAction::triggered, [this](){ captureCatalog(); });
    connect(ui.captureFleetDetail, &QAction::triggered, [this](){ captureFleetDetail(); });
#else
    //艦隊詳細
    connect(ui.captureFleetDetail, &QAction::triggered, [this](){
        openManualCaptureFleetDetail();
    });
    connect(m_fleetDetailDialog, &FleetDetailDialog::finishedCaptureImages, [this](FleetDetailDialog::NextOperationType next, QStringList file_list, int item_width, int item_height, int columns){
        finishManualCaptureFleetDetail(next, file_list, item_width, item_height, columns);
    });
    connect(m_fleetDetailDialog, &FleetDetailDialog::finished, [this](int result) {
        Q_UNUSED(result);
        finishManualCaptureFleetDetail(FleetDetailDialog::None, QStringList(), 0, 0, 1);
    });
#endif
    //艦隊リスト
    connect(ui.captureFleetList, &QAction::triggered, [this](){
        recordingThread.stopRecording();
//        openManualCaptureFleetList();
    });
    connect(ui.reload, &QAction::triggered, ui.webView, &QWebView::reload);
    connect(ui.exit, &QAction::triggered, q, &MainWindow::close);
    connect(ui.actionReturn_to_Kan_Colle, &QAction::triggered, [this]() {
        //艦これ読込み
        ui.webView->load(QUrl(URL_KANCOLLE));
    });
    connect(ui.actionClearAccessCache, &QAction::triggered, [this]() {
        //キャッシュをクリア
        ui.webView->page()->networkAccessManager()->cache()->clear();
    });
    //画像リスト
    connect(ui.viewMemory, &QAction::triggered, [this]() { openMemoryDialog(); });
    //通知タイマー
    connect(ui.notificationTimer, &QAction::triggered, [this]() { m_timerDialog->show(); });
    //設定ダイアログ表示
    connect(ui.preferences, &QAction::triggered, [this]() { openSettingDialog(); });
    //アバウト
    connect(ui.about, &QAction::triggered, [this]() { openAboutDialog(); });
    //ツールバーの表示/非表示
    connect(ui.actionViewToolBar, &QAction::changed, [this](){
        ui.toolBar->setVisible(ui.actionViewToolBar->isChecked());
    });


    ///////////////////////////////////////////////////////////////
    /// 録画
    ///////////////////////////////////////////////////////////////

    //起動時は必ず非表示
    ui.recordLayout->setVisible(false);
    //録画
    connect(ui.actionRecord, &QAction::triggered, [this]() {
        ui.recordLayout->setVisible(!ui.recordLayout->isVisible());
    });
    //録画設定
    connect(ui.actionRecordPreferences, &QAction::triggered, [this](){
        openRecordSettingDialog();
    });
    connect(ui.recordPreferencesButton, &QPushButton::clicked, [this](bool checked){
        Q_UNUSED(checked);
        openRecordSettingDialog();
    });
    //録画開始停止
    connect(ui.recordButton, &QPushButton::clicked, [this](bool checked){
        qDebug() << "click record button";
        Q_UNUSED(checked);

        settings.beginGroup(SETTING_RECORD);
        recordingThread.setFps(settings.value(SETTING_RECORD_FPS, 20).toInt());
        recordingThread.setAudioInputName(settings.value(SETTING_RECORD_AUDIO_SOURCE).toString());
        QString save(settings.value(SETTING_RECORD_SAVE_PATH, QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)).toString());
        if(save.right(1).compare("/") != 0){
            save += "/";
        }
        recordingThread.setSavePath(save + makeFileName("mp4", false));
        recordingThread.setToolPath(settings.value(SETTING_RECORD_TOOL_PATH, "ffmpeg").toString());
        recordingThread.setTempPath(settings.value(SETTING_RECORD_TEMP_PATH, recordingThread.tempPath()).toString());
        recordingThread.setSoundOffset(settings.value(SETTING_RECORD_SOUND_OFFSET, 0).toInt());
        dontViewButtleResult = settings.value(SETTING_RECORD_DONT_VIEW_BUTTLE, true).toBool();
        muteTimerSound = settings.value(SETTING_RECORD_MUTE_TIMER_SOUND, true).toBool();
        settings.endGroup();

        if(!recordingThread.isSettingValid()){
            QMessageBox::warning(q, "warning", "Please input recording settings.");
            openRecordSettingDialog();

        }else if(!ui.webView->gameExists()){
            ui.statusBar->showMessage(tr("failed to started recording."), STATUS_BAR_MSG_TIME);

        }else if(recordingThread.status() == RecordingThread::Ready){
//            recordingThread.setToolPath(QStringLiteral("D:\\ffmpeg\\bin\\ffmpeg.exe"));
//            recordingThread.setToolParam(QStringLiteral("-r %1 -i %2 -vcodec libx264 -qscale:v 0 %3"));
//            recordingThread.setSavePath(QStringLiteral("d:\\temp\\rec\\") + makeFileName("mp4", false));
//            recordingThread.setAudioInputName(recordingThread.audioInputNames().at(0));
//            recordingThread.setFps(20);

            //ミュートする
            m_timerDialog->setAlarmMute(muteTimerSound);

            //録音開始
            recordingThread.setWebView(ui.webView);
            recordingThread.startRecording();
        }else if(recordingThread.status() == RecordingThread::Recording){
            //録音停止
            recordingThread.stopRecording();
            //ミュート解除
            m_timerDialog->setAlarmMute(false);
        }else{
        }
    });
    //録画の状態変化
    connect(&recordingThread, &RecordingThread::statusChanged, [this](RecordingThread::RecordingStatus status){
        switch(status){
        case RecordingThread::Ready:
            ui.recordStatusLabel->setText("Ready");
            break;
        case RecordingThread::Recording:
            ui.recordStatusLabel->setText("Recording");
            break;
        case RecordingThread::Saving:
            ui.recordStatusLabel->setText("Saving");
            break;
        case RecordingThread::Convert:
            ui.recordStatusLabel->setText("Convert");
            break;
        default:
            ui.recordStatusLabel->setText("Error");
            break;
        }
    });
    //録画時間
    connect(&recordingThread, &RecordingThread::durationChanged, [this](const qint64 &duration){
        long min = (duration / 1000) / 60;
        long sec = (duration / 1000) % 60;
        ui.recordingTimeLabel->setText(QString("%1:%2").arg(min, 2, 10, QLatin1Char('0')).arg(sec, 2, 10, QLatin1Char('0')));
    });
    //録画のテンポラリフォルダ作成
    recordingThread.getTempPath();  //呼べば無ければ作られる

    ///////////////////////////////////////////////////////////////
    /// ブラウザ
    ///////////////////////////////////////////////////////////////

    //フルスクリーン
    q->addAction(ui.actionFullScreen);
    connect(ui.actionFullScreen, &QAction::triggered, [this]() {
        if(q->isFullScreen()){
            //フルスクリーン解除
            q->setWindowState(q->windowState() ^ Qt::WindowFullScreen);
        }else if(ui.webView->gameExists()){
            //フルスクリーンじゃなくてゲームがある
            setSplitWindowVisiblity(false);
            q->setWindowState(q->windowState() ^ Qt::WindowFullScreen);
        }else{
            //フルスクリーンでゲームがないときは何もしない
        }
    });
    //ズーム
    setGameSize(1); //初期状態
    connect(ui.actionZoom050, &QAction::triggered, [this](){ setGameSize(0.5); });
    connect(ui.actionZoom075, &QAction::triggered, [this](){ setGameSize(0.75); });
    connect(ui.actionZoom100, &QAction::triggered, [this](){ setGameSize(1); });
    connect(ui.actionZoom125, &QAction::triggered, [this](){ setGameSize(1.25); });
    connect(ui.actionZoom150, &QAction::triggered, [this](){ setGameSize(1.5); });
    connect(ui.actionZoom175, &QAction::triggered, [this](){ setGameSize(1.75); });
    connect(ui.actionZoom200, &QAction::triggered, [this](){ setGameSize(2); });
    //崩れるのでなし
    ui.actionZoom050->setVisible(false);
    ui.actionZoom075->setVisible(false);


    ///////////////////////////////////////////////////////////////
    /// ウインドウ
    ///////////////////////////////////////////////////////////////

    //ウインドウ分割
    connect(ui.actionSplitWindow, &QAction::triggered, [this]() {
        setSplitWindowVisiblity(!isSplitWindowVisible());
    });
    //左右に分割
    connect(ui.actionHorizontalSplit, &QAction::triggered, [this]() {
        setSplitWindowOrientation(Qt::Horizontal);
        if(!isSplitWindowVisible())
            setSplitWindowVisiblity(true);
    });
    //上下に分割
    connect(ui.actionVerticalSplit, &QAction::triggered, [this]() {
        setSplitWindowOrientation(Qt::Vertical);
        if(!isSplitWindowVisible())
            setSplitWindowVisiblity(true);
    });
    //分割を解除
    connect(ui.actionDisableSplit, &QAction::triggered, [this]() {
        if(isSplitWindowVisible())
            setSplitWindowVisiblity(false);
    });

    //タブウインドウの再読み込み
    connect(ui.actionReloadTab, &QAction::triggered, [this]() {
        if(!isSplitWindowVisible())
            return;
        ui.tabWidget->reloadTab();
    });
    //タブウインドウにタブを追加
    connect(ui.actionAddTab, &QAction::triggered, [this]() {
        if(!isSplitWindowVisible())
            return;
        ui.tabWidget->newTab(QUrl("http://www.google.co.jp"));
    });
    //タブウインドウでタブを削除
    connect(ui.actionRemoveTab, &QAction::triggered, [this]() {
        if(!isSplitWindowVisible())
            return;
        ui.tabWidget->closeTab();
    });
    //タブで検索
    q->addAction(ui.actionFindInTab);
    connect(ui.actionFindInTab, &QAction::triggered, [this]() {
        if(!isSplitWindowVisible())
            return;
        ui.tabWidget->find();
    });
    //タブ移動
    q->addAction(ui.actionTabSwitchPrev);
    q->addAction(ui.actionTabSwitchNext);
    q->addAction(ui.actionTabSwitchPrev2);
    q->addAction(ui.actionTabSwitchNext2);
    connect(ui.actionTabSwitchPrev, &QAction::triggered, [this]() {
        if(!isSplitWindowVisible())
            return;
        ui.tabWidget->prevTab();
    });
    connect(ui.actionTabSwitchNext, &QAction::triggered, [this]() {
        if(!isSplitWindowVisible())
            return;
        ui.tabWidget->nextTab();
    });
    connect(ui.actionTabSwitchPrev2, &QAction::triggered, [this]() {
        if(!isSplitWindowVisible())
            return;
        ui.tabWidget->prevTab();
    });
    connect(ui.actionTabSwitchNext2, &QAction::triggered, [this]() {
        if(!isSplitWindowVisible())
            return;
        ui.tabWidget->nextTab();
    });


    ///////////////////////////////////////////////////////////////
    /// WebView
    ///////////////////////////////////////////////////////////////

    //WebViewをクリック
    connect(ui.webView, &WebView::mousePressed, [this](QMouseEvent *event) {
        //艦隊の被弾状況を調べる
        checkMajorDamageShip(event->localPos());
        //遠征の残り時間を調べる
        checkExpeditionRemainTime(event->localPos());
    });

    //WebViewの読込み開始
    connect(ui.webView, &QWebView::loadStarted, [this](){
       ui.progressBar->show();
    });
    //WebViewの読込み完了
    connect(ui.webView, &QWebView::loadFinished, [this](bool ok) {
        if (ok) {
            ui.statusBar->showMessage(MainWindow::tr("complete"), STATUS_BAR_MSG_TIME);

            setGameSize(1); //元に戻す

//            if(ui.webView->url().toString().compare(URL_KANCOLLE) == 0){
//                qDebug() << "complete:" << ui.webView->url();
//                setFullScreen();
//            }
        } else {
            ui.statusBar->showMessage(MainWindow::tr("error"), STATUS_BAR_MSG_TIME);
        }
        ui.progressBar->hide();
    });
    //WebViewの読込み状態
    connect(ui.webView, &QWebView::loadProgress, ui.progressBar, &QProgressBar::setValue);


    ///////////////////////////////////////////////////////////////
    /// お気に入り
    ///////////////////////////////////////////////////////////////

    //お気に入りの読込み
    m_favorite.load(ui.favorite, true);
    //お気に入りを選択した
    connect(&m_favorite, &FavoriteMenu::selectFav, [this](const QUrl &url){
        if(!isSplitWindowVisible())
            setSplitWindowVisiblity(true);
        ui.tabWidget->openUrl(url);
    });
    //お気に入りの更新
    connect(ui.tabWidget, &TabWidget::updateFavorite, [this]() {
        m_favorite.load(ui.favorite);
    });
    //お気に入りのダウンロード終了
    connect(&m_favorite, &FavoriteMenu::downloadFinished, [this]() {
        m_favorite.load(ui.favorite);
    });


    ///////////////////////////////////////////////////////////////
    /// アップデート
    ///////////////////////////////////////////////////////////////

    //アップデートの確認をする
    m_updateInfoDialog->CheckUpdate();
    connect(ui.actionUpdate, &QAction::triggered, [this](){ m_updateInfoDialog->CheckUpdate(); });
    //アプデート確認の情報を元にお気に入りのアップデート
    connect(m_updateInfoDialog, &UpdateInfoDialog::lastFavoriteUpdateDateChanged, [this](const QString &lastFavoriteUpdateDate){
        m_favorite.updateFromInternet(lastFavoriteUpdateDate);
    });
    //アプデート確認の情報を元にタイマーのアップデート
    connect(m_updateInfoDialog, &UpdateInfoDialog::lastTimerSelectGuideUpdateDateChanged, [this](const QString &lastTimerSelectGuideUpdateDate){
        m_timerDialog->setLastTimerSelectGuideUpdateDate(lastTimerSelectGuideUpdateDate);
    });

    //通知アイコン
#ifdef Q_OS_WIN
    trayIcon.show();
#endif

#ifdef DISABLE_CATALOG_AND_DETAIL_FLEET
    ui.captureCatalog->setVisible(false);
//    ui.captureFleetDetail->setVisible(false);
#endif
}

MainWindow::Private::~Private()
{
    delete m_fleetDetailDialog;
    delete m_updateInfoDialog;
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
QString MainWindow::Private::makeFileName(const QString &format, bool isfull) const
{
    if(isfull){
        return QStringLiteral("%1/kanmusu_%2.%3")
                .arg(settings.value(QStringLiteral("path")).toString())
                .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd_hh-mm-ss-zzz")))
                .arg(format.toLower());
    }else{
        return QStringLiteral("kanmusu_%1.%2")
                .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd_hh-mm-ss-zzz")))
                .arg(format.toLower());
    }
}
//テンポラリのファイル名を作成する
QString MainWindow::Private::makeTempFileName(const QString &format) const
{
    return QStringLiteral("%1/kanmusu_temp%2.%3")
            .arg(QDir::tempPath())
            .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd_hh-mm-ss-zzz")))
            .arg(format.toLower());
}

//思い出を残す
void MainWindow::Private::captureGame(bool andEdit)
{
    qDebug() << "captureGame";

    //設定確認
    checkSavePath();

    //戦績報告のプレビューを一時的に消してキャプチャー
    bool old_buttle_result = ui.viewButtleResult->isVisible();
    ui.viewButtleResult->setVisible(false);
    QImage img = ui.webView->capture();
    ui.viewButtleResult->setVisible(old_buttle_result);
    if (img.isNull()){
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

    if(andEdit){
        //編集もする
        QString tempPath = makeTempFileName(format);
        QString editPath = makeFileName(format);
//        qDebug() << "temp path:" << tempPath;

        //保存する
        ui.statusBar->showMessage(tr("saving to %1...").arg(tempPath), STATUS_BAR_MSG_TIME);
        if (img.save(tempPath, format.toUtf8().constData())) {
            //編集ダイアログ
            openImageEditDialog(path, tempPath, editPath);
        } else {
            ui.statusBar->showMessage(tr("failed save image"), STATUS_BAR_MSG_TIME);
        }
        //テンポラリのファイルを消す
        QFile::remove(tempPath);
    }else{
        //キャプチャーだけ

        //保存する
        ui.statusBar->showMessage(tr("saving to %1...").arg(path), STATUS_BAR_MSG_TIME);
        if (img.save(path, format.toUtf8().constData())) {
            //つぶやくダイアログ
            openTweetDialog(path);
        } else {
            ui.statusBar->showMessage(tr("failed save image"), STATUS_BAR_MSG_TIME);
        }
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
void MainWindow::Private::openTweetDialog(const QString &path, bool force)
{
    //連携を使用しない
    if(force == false && settings.value(SETTING_GENERAL_UNUSED_TWITTER, false).toBool()){
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
//画像リストダイアログを開く
void MainWindow::Private::openMemoryDialog()
{
    checkSavePath();
    MemoryDialog dlg(settings.value(QStringLiteral("path")).toString(), q);
    dlg.exec();
    if(QFile::exists(dlg.imagePath())){
        switch(dlg.nextOperation()){
        case MemoryDialog::Tweet:
            //つぶやく
            openTweetDialog(dlg.imagePath(), true);
            break;
        case MemoryDialog::Edit:
        {
            //編集
            QString format;
            if(settings.value(SETTING_GENERAL_SAVE_PNG, false).toBool())
                format = QStringLiteral("png");
            else
                format = QStringLiteral("jpg");
            QString path = makeFileName(format);
            QString editPath = makeFileName(format);
            openImageEditDialog(path ,dlg.imagePath(), editPath);
            break;
        }
        default:
            break;
        }
    }
}
//設定ダイアログを開く
void MainWindow::Private::openSettingDialog()
{
    SettingsDialog dlg(q);
    dlg.setSavePath(settings.value(QStringLiteral("path")).toString());
    dlg.setUnusedTwitter(settings.value(SETTING_GENERAL_UNUSED_TWITTER, false).toBool());
    dlg.setSavePng(settings.value(SETTING_GENERAL_SAVE_PNG, false).toBool());
    dlg.setMaskAdmiralName(settings.value(SETTING_GENERAL_MASK_ADMIRAL_NAME, false).toBool());
    dlg.setMaskHqLevel(settings.value(SETTING_GENERAL_MASK_HQ_LEVEL, false).toBool());
    dlg.setProxyEnable(settings.value(SETTING_GENERAL_PROXY_ENABLE, false).toBool());
    dlg.setProxyHost(settings.value(SETTING_GENERAL_PROXY_HOST).toString());
    dlg.setProxyPort(settings.value(SETTING_GENERAL_PROXY_PORT, 8888).toInt());
    dlg.setUseCookie(settings.value(SETTING_GENERAL_USE_COOKIE, true).toBool());
    dlg.setDisableContextMenu(settings.value(SETTING_GENERAL_DISABLE_CONTEXT_MENU, false).toBool());
    dlg.setDisableExitShortcut(settings.value(SETTING_GENERAL_DISABLE_EXIT, DISABLE_EXIT_DEFAULT).toBool());
    dlg.setViewButtleResult(settings.value(QStringLiteral(SETTING_GENERAL_VIEW_BUTTLE_RESULT), true).toBool());
    dlg.setButtleResultPosition(static_cast<SettingsDialog::ButtleResultPosition>(settings.value(QStringLiteral(SETTING_GENERAL_BUTTLE_RESULT_POSITION), 1).toInt()));
    dlg.setButtleResultOpacity(settings.value(QStringLiteral(SETTING_GENERAL_VIEW_BUTTLE_RESULT_OPACITY), 0.75).toReal());
    dlg.setTimerAutoStart(settings.value(QStringLiteral(SETTING_GENERAL_TIMER_AUTO_START), true).toBool());
    settings.beginGroup(QStringLiteral(SETTING_TIMER));
    dlg.setTweetFinished(settings.value(QStringLiteral(SETTING_TIMER_TWEETFINISHED),false).toBool());
    settings.endGroup();

    if (dlg.exec()) {
        //設定更新
        settings.setValue(QStringLiteral("path"), dlg.savePath());
        settings.setValue(SETTING_GENERAL_UNUSED_TWITTER, dlg.unusedTwitter());
        settings.setValue(SETTING_GENERAL_SAVE_PNG, dlg.savePng());
        settings.setValue(SETTING_GENERAL_MASK_ADMIRAL_NAME, dlg.isMaskAdmiralName());
        settings.setValue(SETTING_GENERAL_MASK_HQ_LEVEL, dlg.isMaskHqLevel());
        settings.setValue(SETTING_GENERAL_PROXY_ENABLE, dlg.isProxyEnable());
        settings.setValue(SETTING_GENERAL_PROXY_HOST, dlg.proxyHost());
        settings.setValue(SETTING_GENERAL_PROXY_PORT, dlg.proxyPort());
        settings.setValue(SETTING_GENERAL_USE_COOKIE, dlg.useCookie());
        settings.setValue(SETTING_GENERAL_DISABLE_CONTEXT_MENU, dlg.disableContextMenu());
        settings.setValue(SETTING_GENERAL_DISABLE_EXIT, dlg.disableExitShortcut());
        settings.setValue(SETTING_GENERAL_VIEW_BUTTLE_RESULT, dlg.viewButtleResult());
        settings.setValue(SETTING_GENERAL_BUTTLE_RESULT_POSITION, static_cast<int>(dlg.buttleResultPosition()));
        settings.setValue(SETTING_GENERAL_VIEW_BUTTLE_RESULT_OPACITY, dlg.buttleResultOpacity());
        settings.setValue(SETTING_GENERAL_TIMER_AUTO_START, dlg.timerAutoStart());

        //戦果報告の表示位置などを更新
        ui.viewButtleResult->setVisible(ui.viewButtleResult->isVisible() & dlg.viewButtleResult());
        setButtleResultPosition();
        if(ui.viewButtleResult->isVisible()){
            checkMajorDamageShip(QPointF(0,0), true);
        }

        //タイマーの時間でつぶやく
        if(m_timerDialog != NULL){
            //ダイアログとの連携の関係で保存はダイアログのクラスでする
            m_timerDialog->setTweetFinished(dlg.tweetFinished());
        }

        //設定反映（必要なの）
        //プロキシ
        updateProxyConfiguration();
        //右クリックメニュー無効
        ui.webView->setDisableContextMenu(dlg.disableContextMenu());
        //Ctrl+Qを無効化
        if(dlg.disableExitShortcut()){
            ui.exit->setShortcut(QKeySequence());
        }else{
            ui.exit->setShortcut(QKeySequence(QStringLiteral("Ctrl+Q")));
        }
    }
}
//録画の設定ダイアログを開く
void MainWindow::Private::openRecordSettingDialog()
{
    RecordingDialog dlg(q);

    settings.beginGroup(SETTING_RECORD);
    dlg.setFps(settings.value(SETTING_RECORD_FPS, 20).toInt());
    dlg.setAudioSource(settings.value(SETTING_RECORD_AUDIO_SOURCE, recordingThread.audioInputNames().at(0)).toString());
    dlg.setSavePath(settings.value(SETTING_RECORD_SAVE_PATH, QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)).toString());
    dlg.setToolPath(settings.value(SETTING_RECORD_TOOL_PATH, "ffmpeg").toString());
    dlg.setTempPath(settings.value(SETTING_RECORD_TEMP_PATH, recordingThread.tempPath()).toString());
    dlg.setSoundOffset(settings.value(SETTING_RECORD_SOUND_OFFSET, 0).toInt());
    dlg.setDontViewButtleResult(settings.value(SETTING_RECORD_DONT_VIEW_BUTTLE, true).toBool());
    dlg.setMuteNotificationSound(settings.value(SETTING_RECORD_MUTE_TIMER_SOUND, true).toBool());
    settings.endGroup();

    dlg.setDefaultFps(20);
    dlg.setDefaultAudioSource(recordingThread.audioInputNames().at(0));
    dlg.setDefaultSavePath(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
    dlg.setDefaultToolPath("ffmpeg");
    dlg.setDefaultTempPath(recordingThread.tempPath());
    dlg.setDefaultSoundOffset(0);
    dlg.setDefaultDontViewButtleResult(true);
    dlg.setDefaultMuteNotificationSound(true);

    if(dlg.exec()){
        settings.beginGroup(SETTING_RECORD);
        settings.setValue(SETTING_RECORD_FPS, dlg.fps());
        settings.setValue(SETTING_RECORD_AUDIO_SOURCE, dlg.audioSource());
        settings.setValue(SETTING_RECORD_SAVE_PATH, dlg.savePath());
        settings.setValue(SETTING_RECORD_TOOL_PATH, dlg.toolPath());
        settings.setValue(SETTING_RECORD_TEMP_PATH, dlg.tempPath());
        settings.setValue(SETTING_RECORD_SOUND_OFFSET, dlg.soundOffset());
        settings.setValue(SETTING_RECORD_DONT_VIEW_BUTTLE, dlg.dontViewButtleResult());
        settings.setValue(SETTING_RECORD_MUTE_TIMER_SOUND, dlg.muteNotificationSound());
        settings.endGroup();

        dontViewButtleResult = dlg.dontViewButtleResult();
    }

}
//Update proxy setting.
void MainWindow::Private::updateProxyConfiguration()
{
    // Update proxy setting.
    QNetworkAccessManager * accessmanager = ui.webView->page()->networkAccessManager();
    QNetworkProxy * proxy = new QNetworkProxy();

    bool enable = settings.value(SETTING_GENERAL_PROXY_ENABLE, false).toBool();
    QString host = settings.value(SETTING_GENERAL_PROXY_HOST).toString();
    if(host.length() > 0 && enable)
    {
        proxy->setType(QNetworkProxy::HttpProxy);
        proxy->setHostName(host);
        proxy->setPort(settings.value(SETTING_GENERAL_PROXY_PORT, 8888).toInt());

        accessmanager->setProxy(*proxy);
    }
    else
    {
        accessmanager->setProxy(QNetworkProxy::NoProxy);
    }
}
//アバウトダイアログを開く
void MainWindow::Private::openAboutDialog()
{
    AboutDialog dlg(q);
    dlg.setVersion(KANMEMO_VERSION);
    dlg.setDevelopers(KANMEMO_DEVELOPERS);
    dlg.exec();
}
//編集ダイアログを開く
void MainWindow::Private::openImageEditDialog(const QString &path, const QString &tempPath, const QString &editPath)
{
    ImageEditDialog dlg(path, tempPath, editPath, q);
    dlg.exec();
    if(QFile::exists(dlg.editImagePath()) && dlg.nextOperation() == ImageEditDialog::SaveAndTweet){
        //つぶやく
        openTweetDialog(dlg.editImagePath());
    }
}

//艦隊詳細を作成するダイアログを表示する
void MainWindow::Private::openManualCaptureFleetDetail()
{
    //設定確認
    checkSavePath();

    //メニューを一時停止
    ui.capture->setEnabled(false);
    ui.actionCaptureAndEdit->setEnabled(false);
    ui.captureCatalog->setEnabled(false);
    ui.captureFleetDetail->setEnabled(false);
    ui.captureFleetList->setEnabled(false);

    //設定変更
    QStringList fdd_msg_list;
    fdd_msg_list << tr("Please to capture with a detailed view of the ship in the organization screen.");
    fdd_msg_list << tr("The following buttons appear when you capture.");
    fdd_msg_list << tr("Combine the image you have captured on completion.");
    m_fleetDetailDialog->setParameters(DETAIL_RECT_CAPTURE, 0.3, 2, 6, fdd_msg_list);
    //クリア
    m_fleetDetailDialog->clear();
    //表示
    m_fleetDetailDialog->setWindowTitle(tr("Fleet Detail"));
    m_fleetDetailDialog->show();
    m_fleetDetailDialog->raise();
    m_fleetDetailDialog->activateWindow();
}
//艦隊リストを作成するダイアログを表示する
void MainWindow::Private::openManualCaptureFleetList()
{
    //設定確認
    checkSavePath();

    //メニューを一時停止
    ui.capture->setEnabled(false);
    ui.actionCaptureAndEdit->setEnabled(false);
    ui.captureCatalog->setEnabled(false);
    ui.captureFleetDetail->setEnabled(false);
    ui.captureFleetList->setEnabled(false);

    //設定変更
    QStringList fdd_msg_list;
    fdd_msg_list << tr("Please be captured by a selection list of warships in the scheduling screen.");
    fdd_msg_list << tr("The following buttons appear when you capture.");
    fdd_msg_list << tr("Combine the image you have captured on completion.");
    m_fleetDetailDialog->setParameters(FLEETLIST_RECT_CAPTURE, 0.3, 4, 20, fdd_msg_list);
    //クリア
    m_fleetDetailDialog->clear();
    //表示
    m_fleetDetailDialog->setWindowTitle(tr("Fleet List"));
    m_fleetDetailDialog->show();
    m_fleetDetailDialog->raise();
    m_fleetDetailDialog->activateWindow();
}
//艦隊詳細作成ダイアログ終了時の処理
void MainWindow::Private::finishManualCaptureFleetDetail(FleetDetailDialog::NextOperationType next, QStringList file_list, int item_width, int item_height, int columns)
{
    if(next == FleetDetailDialog::Combine){
        QString path = combineImage(file_list, item_width, item_height, columns);
        if(path.length() > 0)
            openTweetDialog(path);
    }

    //メニューを復活
    ui.capture->setEnabled(true);
    ui.actionCaptureAndEdit->setEnabled(true);
    ui.captureCatalog->setEnabled(true);
    ui.captureFleetDetail->setEnabled(true);
    ui.captureFleetList->setEnabled(true);
}
//ゲーム画面へクリックイベントを送る
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
//画像を結合する（主に艦隊詳細）
QString MainWindow::Private::combineImage(QStringList file_list, int item_width, int item_height, int columns)
{
    if(file_list.length() == 0 || columns < 1)
        return QString();

    int rows = qCeil(static_cast<qreal>(file_list.length()) / static_cast<qreal>(columns));
    int x = 0;
    int y = 0;
    //画像の数が列数より小さい時は列数を調節する
    if(file_list.length() < columns){
        columns = file_list.length();
    }
    //結合画像を作る
    QImage resultImage(item_width * columns, item_height * rows, QImage::Format_ARGB32);
    QPainter painter(&resultImage);
    painter.fillRect(resultImage.rect(), "#ece3d4");
    for(int i=0; i<file_list.length(); i++){
        QImage item = QImage(file_list[i]);
        painter.drawImage(x, y, item);
        if((i % columns) == (columns - 1)){
            x = 0;
            y += item_height;
        }else{
            x += item_width;
        }
    }

    //パスをつくる
    QString format;
    if(settings.value(SETTING_GENERAL_SAVE_PNG, false).toBool())
        format = QStringLiteral("png");
    else
        format = QStringLiteral("jpg");
    QString path = makeFileName(format);

    //保存
    resultImage.save(path);

    return path;
}
//分割ウインドウの表示切り替え
void MainWindow::Private::setSplitWindowVisiblity(bool visible)
{
    if(visible){
        //→表示

        //タブがひとつも無ければ追加
        if(ui.tabWidget->count() == 0){
            ui.tabWidget->newTab(QUrl("http://www.google.co.jp/"));
        }
    }else{
        //→非表示
        bakSplitterSizes = ui.splitter->sizes();    //非表示する前に保存しておく
    }
    //表示状態をひっくり返す
    ui.splitter->widget(SPLIT_WEBPAGE_INDEX)->setVisible(visible);

    //            ui.contentSplitter->setOrientation(Qt::Vertical);
    //    qDebug() << "splitter size:" << ui.splitter->sizes();
}
//分割ウインドウの表示状態
bool MainWindow::Private::isSplitWindowVisible()
{
    return ui.splitter->widget(SPLIT_WEBPAGE_INDEX)->isVisible();
}
//分割方向を変える
void MainWindow::Private::setSplitWindowOrientation(Qt::Orientation orientation)
{
    ui.splitter->setOrientation(orientation);

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
    ui.webView->page()->mainFrame()->setScrollPosition(QPoint(0, 0));
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
//ゲームサイズを調整する（メニューも）
void MainWindow::Private::setGameSize(qreal factor)
{
    //サイズ調整
    ui.webView->setGameSizeFactor(factor);
    //メニュー調整
    ui.actionZoom050->setChecked(factor == 0.5);
    ui.actionZoom075->setChecked(factor == 0.75);
    ui.actionZoom100->setChecked(factor == 1);
    ui.actionZoom125->setChecked(factor == 1.25);
    ui.actionZoom150->setChecked(factor == 1.5);
    ui.actionZoom175->setChecked(factor == 1.75);
    ui.actionZoom200->setChecked(factor == 2);
}
//戦果報告画面での大破判定
void MainWindow::Private::checkMajorDamageShip(const QPointF &pos, bool force)
{
    if(!settings.value(QStringLiteral(SETTING_GENERAL_VIEW_BUTTLE_RESULT), true).toBool()
            || q->isFullScreen()){
        return;
    }
    if(dontViewButtleResult && recordingThread.status() == RecordingThread::Recording){
        //録画中は表示しないがONで録画中
        return;
    }
    qreal opacity = settings.value(QStringLiteral(SETTING_GENERAL_VIEW_BUTTLE_RESULT_OPACITY), 0.75).toReal();

    bool old = ui.viewButtleResult->isVisible();
    ui.viewButtleResult->setVisible(false);
    QImage img = ui.webView->capture(false);
    ui.viewButtleResult->setVisible(old);
    if(img.isNull()){
        return;
    }

    GameScreen gameScreen(img);
    switch(gameScreen.screenType()){
    case GameScreen::ButtleResultScreen:
        if(!ui.viewButtleResult->isVisible() || force){
            //非表示→表示
            QImage background;
            if(gameScreen.isContainMajorDamageShip()){
                //大破が含まれるっぽい
                background.load(":/resources/ButtleResultBackgroundRed.png");
            }else{
                background.load(":/resources/ButtleResultBackgroundBlue.png");
            }
            QImage buttle(":/resources/ButtleResultBackgroundTrans.png");
            QPainter painter(&buttle);
            painter.setOpacity(opacity);
            painter.drawImage(0, 0, background);
            painter.drawImage(20, 20, img.scaled(300, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui.viewButtleResult->setPixmap(QPixmap::fromImage(buttle));
            ui.viewButtleResult->setWindowOpacity(0.5);
            ui.viewButtleResult->setVisible(true);
        }
        break;

    case GameScreen::GoOrBackScreen:
    {
        //進撃or撤退
        QRect game_rect = ui.webView->getGameRect();
#if 0
        //思ったより当たり判定でかいからボタン単位は面倒なだけ
        QRect go_rect(game_rect.x() + BUTTLE_GO_BUTTON_RECT.x()
                      , game_rect.y() + BUTTLE_GO_BUTTON_RECT.y()
                      , BUTTLE_GO_BUTTON_RECT.width()
                      , BUTTLE_GO_BUTTON_RECT.height());
        QRect back_rect(game_rect.x() + BUTTLE_BACK_BUTTON_RECT.x()
                        , game_rect.y() + BUTTLE_BACK_BUTTON_RECT.y()
                        , BUTTLE_BACK_BUTTON_RECT.width()
                        , BUTTLE_BACK_BUTTON_RECT.height());
        if(go_rect.contains(pos.x(), pos.y()) || back_rect.contains(pos.x(), pos.y())){
#else
        QRect button_rect(game_rect.x() + BUTTLE_GO_OR_BACK_RECT.x()
                          , game_rect.y() + BUTTLE_GO_OR_BACK_RECT.y()
                          , BUTTLE_GO_OR_BACK_RECT.width()
                          , BUTTLE_GO_OR_BACK_RECT.height());
        if(button_rect.contains(pos.x(), pos.y())){
#endif
        //判定範囲内をクリックしてたら消す
            ui.viewButtleResult->setVisible(false);
        }
        break;
    }
    case GameScreen::TurnCompassScreen:
        //羅針盤を回す
        ui.viewButtleResult->setVisible(false);
        break;

    default:
        if(gameScreen.isVisible(GameScreen::HeaderPart)){
            //ヘッダーが表示されてたら母港画面
            ui.viewButtleResult->setVisible(false);
        }
        break;
    }

}
//遠征の残り時間を調べる
void MainWindow::Private::checkExpeditionRemainTime(const QPointF &pos)
{
    if(!settings.value(QStringLiteral(SETTING_GENERAL_TIMER_AUTO_START), true).toBool()){
        return;
    }
    QImage img = ui.webView->capture(false);
    if(img.isNull()){
        return;
    }
    QRect game_rect = ui.webView->getGameRect();
    QPointF game_pos(pos.x() - game_rect.x(), pos.y() - game_rect.y());
    static int last_click_fleet_no = -1;
    static qint64 last_select_total = -1;
    //0:item選択
    //1:item決定押した
    //2:遠征開始を押した（元画面に戻ってくる）
    GameScreen gameScreen(img);
    if(gameScreen.screenType() == GameScreen::ExpeditionScreen){

        qDebug() << "Expedition";

        //時間の取得
        qint64 total;
        qint64 remain;
        gameScreen.getExpeditionTime(&total, &remain);
        qDebug() << "  checkExpeditionRemainTime: total=" << total << ", remain=" << remain;

        //項目選択済みなら時間を設定
        if((last_click_fleet_no != -1) && (total != -1) && (remain != -1)){
            if(m_timerDialog != NULL){
                m_timerDialog->updateTimerSetting(1, last_click_fleet_no, remain, total);
            }
            last_click_fleet_no = -1;
        }

        //メモ：遠征出撃からの時は取得できるまで項目選択をしない？

        //項目選択したか確認
        last_click_fleet_no = gameScreen.getClickExpeditionItemFleetNo(game_pos);
        qDebug() << "  checkExpeditionRemainTime:" << last_click_fleet_no;

        //出撃済み遠征を選択してなくて決定を押した？かつ、トータルが取得できて残りが取れないとき
        if((last_click_fleet_no == -1)
                && EXPEDITION_ITEM_COMFIRM_RECT.contains(game_pos.x(), game_pos.y())
                && (total != -1)
                && (remain == -1)){
            qDebug() << "  click comfirm " << total;
            last_select_total = total;
        }

    }else if(last_select_total != -1){
        //決定が押されて艦隊選択になっているはず
        qDebug() << "  select fleet and start.";
        if(gameScreen.isClickExpeditionStartButton(game_pos)){
            //開始ボタンを押した
            qDebug() << "    click start";
            //艦隊番号確認
            int fleet_no = gameScreen.getExpeditionFleetNo();
            //設定を投げる
            if(m_timerDialog != NULL){
                m_timerDialog->updateTimerSetting(1, fleet_no, last_select_total, last_select_total);
            }
            //クリア
            last_select_total = -1;
        }
    }else{
        last_click_fleet_no = -1;
        last_select_total = -1;
    }
}

//Webページに関連する設定をする
void MainWindow::Private::setWebSettings()
{
    //WebViewの設定（クッキー）
    ui.actionClearCookies->setEnabled(false);
    if(notUseCookie){
        //強制無効化
        qDebug() << "not use cookie";
    }else if(settings.value(SETTING_GENERAL_USE_COOKIE, true).toBool()){
        CookieJar* jar = new CookieJar(q);
        ui.webView->page()->networkAccessManager()->setCookieJar(jar);
        connect(ui.actionClearCookies, SIGNAL(triggered()), jar, SLOT(deleteAll()));
        ui.actionClearCookies->setEnabled(true);
    }
    //WebViewの設定（キャッシュ）
    QNetworkDiskCache *cache = new QNetworkDiskCache(q);
    cache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)+CACHE_LOCATION_SUFFIX);
    cache->setMaximumCacheSize(1073741824); //about 1024MB
    ui.webView->page()->networkAccessManager()->setCache(cache);
    ui.tabWidget->setCache(cache);
    //古いキャッシュフォルダを削除
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if(dir.exists()){
        dir.removeRecursively();
    }

    QWebSettings *websetting = QWebSettings::globalSettings();
    //JavaScript関連設定
    websetting->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    websetting->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
    websetting->setAttribute(QWebSettings::PluginsEnabled, true);
    websetting->setAttribute(QWebSettings::JavascriptEnabled, true);
    //フォント設定
#if defined(Q_OS_WIN32)
//    websetting->setFontFamily(QWebSettings::StandardFont, "ＭＳ Ｐゴシック");
//    websetting->setFontFamily(QWebSettings::StandardFont, "MS PGothic");
    websetting->setFontFamily(QWebSettings::StandardFont, "Meiryo UI");
    websetting->setFontFamily(QWebSettings::SerifFont, "MS PMincho");
    websetting->setFontFamily(QWebSettings::SansSerifFont, "MS PGothic");
    websetting->setFontFamily(QWebSettings::FixedFont, "MS Gothic");
#elif defined(Q_OS_LINUX)
#elif defined(Q_OS_MAC)
    websetting->setFontFamily(QWebSettings::StandardFont, "ヒラギノ角ゴPro");
    websetting->setFontFamily(QWebSettings::SerifFont, "ヒラギノ明朝Pro");
    websetting->setFontFamily(QWebSettings::SansSerifFont, "ヒラギノ角ゴPro");
    websetting->setFontFamily(QWebSettings::FixedFont, "Osaka");
#else
#endif
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    updateProxyConfiguration();
}
//ダイアログの作成をする
void MainWindow::Private::makeDialog()
{
    //通知タイマーのダイアログ作成
    m_timerDialog = new TimerDialog(q, &trayIcon, &settings);
    //アップデート通知のダイアログ作成
    m_updateInfoDialog = new UpdateInfoDialog(q, &settings);
    //艦隊詳細作成のダイアログ作成
    QStringList fdd_msg_list;
//    fdd_msg_list << tr("Please to capture with a detailed view of the ship in the organization screen.");
//    fdd_msg_list << tr("The following buttons appear when you capture.");
//    fdd_msg_list << tr("Combine the image you have captured on completion.");
    m_fleetDetailDialog = new FleetDetailDialog(ui.webView
                                                , DETAIL_RECT_CAPTURE
                                                , 0.3           //0.3倍
                                                , 2             //2列
                                                , 6             //最大6
                                                , fdd_msg_list  //説明文言リスト
                                                , &settings
                                                , q);
}
//戦果報告の表示位置
void MainWindow::Private::setButtleResultPosition()
{
    SettingsDialog::ButtleResultPosition position = static_cast<SettingsDialog::ButtleResultPosition>(settings.value(QStringLiteral(SETTING_GENERAL_BUTTLE_RESULT_POSITION), 1).toInt());

    switch(position){
    case SettingsDialog::LeftTop:
        ui.additionalInfoHSpacerLeft->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
        ui.additionalInfoVSpacerTop->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
        ui.additionalInfoHSpacerRight->changeSize(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        ui.additionalInfoVSpacerBottom->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        break;
    default:
    case SettingsDialog::RightTop:
        ui.additionalInfoHSpacerLeft->changeSize(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        ui.additionalInfoVSpacerTop->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
        ui.additionalInfoHSpacerRight->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
        ui.additionalInfoVSpacerBottom->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        break;
    case SettingsDialog::LeftBottom:
        ui.additionalInfoHSpacerLeft->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
        ui.additionalInfoVSpacerTop->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        ui.additionalInfoHSpacerRight->changeSize(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        ui.additionalInfoVSpacerBottom->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
        break;
    case SettingsDialog::RightBottom:
        ui.additionalInfoHSpacerLeft->changeSize(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        ui.additionalInfoVSpacerTop->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        ui.additionalInfoHSpacerRight->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
        ui.additionalInfoVSpacerBottom->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
        break;
    case SettingsDialog::Center:
        ui.additionalInfoHSpacerLeft->changeSize(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        ui.additionalInfoVSpacerTop->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        ui.additionalInfoHSpacerRight->changeSize(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        ui.additionalInfoVSpacerBottom->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        break;
    }
    //変更反映
    ui.additionalInfoLayout->invalidate();
}


MainWindow::MainWindow(QWidget *parent, bool not_use_cookie)
    : QMainWindow(parent)
    , d(new Private(this, not_use_cookie))
{
    //ウインドウの位置を復元
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, KANMEMO_PROJECT, KANMEMO_NAME);
    settings.beginGroup(QStringLiteral(SETTING_MAINWINDOW));
    restoreGeometry(settings.value(QStringLiteral(SETTING_WINDOW_GEO)).toByteArray());
    restoreState(settings.value(QStringLiteral(SETTING_WINDOW_STATE)).toByteArray());
    //分割ウインドウのサイズ調整
    d->bakSplitterSizes = TimerData::toIntList(settings.value(QStringLiteral(SETTING_SPLITTER_SIZES), QList<QVariant>() << 900 << 300).toList());
    d->ui.splitter->setSizes(d->bakSplitterSizes);
    d->ui.splitter->widget(SPLIT_WEBPAGE_INDEX)->setVisible(settings.value(QStringLiteral(SETTING_SPLITTER_ON)).toBool());
    d->ui.splitter->setOrientation(static_cast<Qt::Orientation>(settings.value(QStringLiteral(SETTING_SPLITTER_ORIENTATION), Qt::Horizontal).toInt()));
    settings.endGroup();

    //拡縮の設定を復元
    //d->setGameSize(settings.value(QStringLiteral(SETTING_GENERAL_ZOOM_FACTOR), 1.0).toReal());

    //Ctrl+Qを無効化
    if(d->settings.value(SETTING_GENERAL_DISABLE_EXIT, DISABLE_EXIT_DEFAULT).toBool()){
        d->ui.exit->setShortcut(QKeySequence());
    }else{
        d->ui.exit->setShortcut(QKeySequence(QStringLiteral("Ctrl+Q")));
    }

    //SSLエラー
    connect(d->ui.webView->page()->networkAccessManager(),
            SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
            this, SLOT(handleSslErrors(QNetworkReply*,QList<QSslError>)));
    //艦これ読込み
    d->ui.webView->load(QUrl(URL_KANCOLLE));

    //タブを復元
    d->ui.tabWidget->load();

    //閉じるときにプライベートを破棄
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
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, KANMEMO_PROJECT, KANMEMO_NAME);
    settings.beginGroup(QStringLiteral(SETTING_MAINWINDOW));
    settings.setValue(QStringLiteral(SETTING_WINDOW_GEO), saveGeometry());
    settings.setValue(QStringLiteral(SETTING_WINDOW_STATE), saveState());
    //分割ウインドウのサイズ調整
    settings.setValue(QStringLiteral(SETTING_SPLITTER_ON), d->ui.splitter->widget(SPLIT_WEBPAGE_INDEX)->isVisible());
    if(d->ui.splitter->widget(SPLIT_WEBPAGE_INDEX)->isVisible()){
        settings.setValue(QStringLiteral(SETTING_SPLITTER_SIZES), TimerData::toList<QVariant, int>(d->ui.splitter->sizes()));
    }else{
        settings.setValue(QStringLiteral(SETTING_SPLITTER_SIZES), TimerData::toList<QVariant, int>(d->bakSplitterSizes));
    }
    settings.setValue(QStringLiteral(SETTING_SPLITTER_ORIENTATION), static_cast<int>(d->ui.splitter->orientation()));
    settings.endGroup();

    //拡縮の設定の保存
    //settings.setValue(QStringLiteral(SETTING_GENERAL_ZOOM_FACTOR), d->ui.webView->getGameSizeFactor());

    //タブの保存
    d->ui.tabWidget->save();

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
    Q_UNUSED(event);
    static bool prev = false;

    if(prev != isFullScreen()){
        d->setFullScreen();
    }
    prev = isFullScreen();
}
