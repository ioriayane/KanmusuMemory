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
#include "webview.h"

#include <QtCore/QDebug>
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKit/QWebElement>
#include <QMenu>
#include <QWebHitTestResult>
#include <QContextMenuEvent>

class WebView::Private
{
public:
    Private(WebView *parent);

private:
    void showNormal();
    void showFullScreen();
    void showOptionalSize(int width, int height, bool isfullscreen);

private:
    WebView *q;

public:
    ViewMode viewMode;
    QRect defaultRect;

    QWebFrame *searchGameFrame(QWebFrame *frame);

private:
    QHash<QString, QString> naviApp;
    QHash<QString, QString> foot;
    QHash<QString, QString> body;
    QHash<QString, QString> gameFrame;
    QHash<QString, QString> flashWrap;
    QHash<QString, QString> embed;
    QHash<QString, QString> sectionWrap;
    QHash<QString, QString> globalNavi;
    QHash<QString, QString> dmmNtgnavi;
    QHash<QString, QString> dmmNtgnaviInner;

    bool setElementProperty(QWebElement &element, QHash<QString, QString> &properties, QHash<QString, QString> &backup);
};

WebView::Private::Private(WebView *parent)
    : q(parent)
    , viewMode(NormalMode)
{
    //表示モードの変更
    connect(q, &WebView::viewModeChanged, [this](ViewMode viewMode) {
        switch (viewMode) {
        case WebView::NormalMode:
            showNormal();
            break;
        case WebView::FullScreenMode:
            showFullScreen();
            break;
        }
    });
    //ズームの変更
    connect(q, &WebView::gameSizeFactorChanged, [this](qreal factor) {
        if(!defaultRect.isValid()){
            defaultRect = q->getGameRect();
        }
        showOptionalSize(defaultRect.width() * factor, defaultRect.height() * factor, false);
    });

}

void WebView::Private::showFullScreen()
{
    //normal -> full
    showOptionalSize(q->window()->width(), q->window()->height(), true);
}

void WebView::Private::showOptionalSize(int width, int height, bool isfull)
{
    //ゲーム画面有るか
    if(!q->gameExists())   return;

    QHash<QString, QString> properties;
    QWebFrame *frame = q->page()->mainFrame();

    ///////////////////////////////////////
    //スクロールバー非表示
    QWebElement element = frame->findFirstElement(QStringLiteral("body"));
    if(!isfull){
        properties.insert(QStringLiteral("overflow"), QStringLiteral(""));
        properties.insert(QStringLiteral("min-width"), QString("%1px").arg(width+188));
    }else{
        //フルスクリーンのみ
        properties.insert(QStringLiteral("overflow"), QStringLiteral("hidden"));
        properties.insert(QStringLiteral("min-width"), QString("%1px").arg(width+188));
    }
    if(!setElementProperty(element, properties, body)){
        qDebug() << "failed find target body";
        return;
    }
    properties.clear();


    /////////////////////////////////////////
    //上のバー消す
    element = frame->findFirstElement(QStringLiteral("#main-ntg"));
    if(!isfull){
        properties.insert(QStringLiteral("position"), QStringLiteral("relative"));
//        properties.insert(QStringLiteral("margin"), QStringLiteral("0 -12px 20px"));
//        properties.insert(QStringLiteral("visibility"), QStringLiteral(""));
        properties.insert(QStringLiteral("top"), QStringLiteral(""));
        properties.insert(QStringLiteral("left"), QStringLiteral(""));
    }else{
        //フルスクリーンのみ
        properties.insert(QStringLiteral("position"), QStringLiteral("absolute"));
//        properties.insert(QStringLiteral("margin"), QStringLiteral("0"));
//        properties.insert(QStringLiteral("visibility"), QStringLiteral("hidden"));
        properties.insert(QStringLiteral("top"), QStringLiteral("0px"));
        properties.insert(QStringLiteral("left"), QStringLiteral("0px"));
    }
    if(!setElementProperty(element, properties, dmmNtgnavi)){
        qDebug() << "failed find target #dmm-ntgnavi";
        return;
    }
    properties.clear();


    /////////////////////////////////////////
    //フレームを最大化
    element = frame->findFirstElement(QStringLiteral("#game_frame"));
    if (element.isNull()) {
        qDebug() << "failed find target #game_frame";
        //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return;
    }
    if(!isfull){
        //フルスクリーンじゃない
        properties.insert(QStringLiteral("position"), QStringLiteral(""));
        properties.insert(QStringLiteral("top"), QStringLiteral(""));
        properties.insert(QStringLiteral("left"), QStringLiteral(""));
        properties.insert(QStringLiteral("width"), QString("%1px").arg(width+100));
        properties.insert(QStringLiteral("height"), QString("%1px").arg(height+100));
        properties.insert(QStringLiteral("z-index"), QStringLiteral(""));
    }else{
        properties.insert(QStringLiteral("position"), QStringLiteral("absolute"));
        properties.insert(QStringLiteral("top"), QStringLiteral("0px"));
        properties.insert(QStringLiteral("left"), QStringLiteral("0px"));
        properties.insert(QStringLiteral("width"), QString("%1px").arg(width+100));
        properties.insert(QStringLiteral("height"), QString("%1px").arg(height+100));
        properties.insert(QStringLiteral("z-index"), QStringLiteral("10"));
    }
    if(!setElementProperty(element, properties, gameFrame)){
        qDebug() << "failed find target game_frame tag";
        return;
    }
    properties.clear();


    /////////////////////////////////////////
    //フレームの子供からflashの入ったdivを探して、さらにその中のembedタグを調べる
    frame = searchGameFrame(frame);
    element = frame->findFirstElement(QStringLiteral("#flashWrap"));
    if(!isfull){
        //フルスクリーンじゃない
        properties.insert(QStringLiteral("position"), QStringLiteral(""));
        properties.insert(QStringLiteral("top"), QStringLiteral(""));
        properties.insert(QStringLiteral("left"), QStringLiteral(""));
        properties.insert(QStringLiteral("width"), QString("%1px").arg(width));
        properties.insert(QStringLiteral("height"), QString("%1px").arg(height));
    }else{
        properties.insert(QStringLiteral("position"), QStringLiteral("absolute"));
        properties.insert(QStringLiteral("top"), QStringLiteral("0px"));
        properties.insert(QStringLiteral("left"), QStringLiteral("0px"));
        properties.insert(QStringLiteral("width"), QString("%1px").arg(width));
        properties.insert(QStringLiteral("height"), QString("%1px").arg(height));
    }
    if(!setElementProperty(element, properties, flashWrap)){
        qDebug() << "failed find target flashWrap";
        return;
    }
    properties.clear();

    /////////////////////////////////////////
    //embedタグを探す
    element = element.findFirst(QStringLiteral("embed"));
    if (element.isNull()) {
        qDebug() << "failed find target embed";
        return;
    }
    properties.insert(QStringLiteral("width"), QString::number(width));
    properties.insert(QStringLiteral("height"), QString::number(height));
    if(embed.isEmpty()){
        foreach (const QString &key, properties.keys()) {
            embed.insert(key, element.attribute(key));
        }
        qDebug() << element.attribute(QStringLiteral("width"))
                 << "," << element.attribute(QStringLiteral("height"))
                 << "->" << width << "," << height;
    }
    foreach (const QString &key, properties.keys()) {
        element.evaluateJavaScript(QString("this.%1='%2'").arg(key).arg(properties.value(key)));
    }
    properties.clear();

    /////////////////////////////////////////
    //解説とか消す
    element = frame->findFirstElement(QStringLiteral("#sectionWrap"));
    if(!isfull){
//        properties.insert(QStringLiteral("width"), QString("%1px").arg(width));
        properties.insert(QStringLiteral("visibility"), QStringLiteral(""));
    }else{
        //フルスクリーンのみ
//        properties.insert(QStringLiteral("width"), QString("%1px").arg(width));
        properties.insert(QStringLiteral("visibility"), QStringLiteral("hidden"));
    }
    if(!setElementProperty(element, properties, sectionWrap)){
        qDebug() << "failed find target #sectionWrap";
        return;
    }
    properties.clear();

    /////////////////////////////////////////
    //フレームの中の上の隙間
    element = frame->findFirstElement(QStringLiteral("#spacing_top"));
    if(!isfull){
        properties.insert(QStringLiteral("height"), QStringLiteral("16px"));
    }else{
        //フルスクリーンのみ
        properties.insert(QStringLiteral("height"), QStringLiteral("0px"));
    }
    if(!setElementProperty(element, properties, globalNavi)){
        qDebug() << "failed find target spacing_top";
        return;
    }
    properties.clear();

}
//ゲームフレームを検索する
QWebFrame *WebView::Private::searchGameFrame(QWebFrame *frame)
{
    for(int i=0;i<frame->childFrames().count();i++){
        qDebug() << "frame name:" << i << ":" << frame->childFrames().at(i)->frameName();
        if(frame->childFrames().at(i)->frameName() == "game_frame"){
            //発見
            frame = frame->childFrames().at(i);
            break;
        }
    }
    return frame;
}

//エレメントにプロパティを設定する（初回はバックアップする）
bool WebView::Private::setElementProperty(QWebElement &element, QHash<QString, QString> &properties, QHash<QString, QString> &backup)
{
    if(element.isNull()){
        qDebug() << "failed find target";
        return false;
    }
    QString text;
    if(backup.isEmpty()){
        foreach (const QString &key, properties.keys()) {
            backup.insert(key, element.styleProperty(key, QWebElement::InlineStyle));

            text.append(QString("%1(%2) ").arg(key).arg(element.styleProperty(key, QWebElement::InlineStyle)));
        }
        qDebug() << text;
    }
    foreach (const QString &key, properties.keys()) {
        if(properties.value(key).length() > 0){
            element.setStyleProperty(key, properties.value(key));
        }
    }

    return true;
}

void WebView::Private::showNormal()
{
    //full -> normal

    QWebFrame *frame = q->page()->mainFrame();

    /////////////////////////////////////////
    //スクロールバー表示
    QWebElement element = frame->findFirstElement(QStringLiteral("body"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        return;
    }
    //もとに戻す
    foreach (const QString &key, body.keys()) {
        element.setStyleProperty(key, body.value(key));
    }

    /////////////////////////////////////////
    //フレーム
    element = frame->findFirstElement(QStringLiteral("#game_frame"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        return;
    }
    //もとに戻す
    foreach (const QString &key, gameFrame.keys()) {
        element.setStyleProperty(key, gameFrame.value(key));
    }

    /////////////////////////////////////////
    //フレームの子供からflashの入ったdivを探して、さらにその中のembedタグを調べる
    frame = searchGameFrame(frame);
    element = frame->findFirstElement(QStringLiteral("#flashWrap"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        return;
    }
    //もとに戻す
    foreach (const QString &key, flashWrap.keys()) {
        element.setStyleProperty(key, flashWrap.value(key));
    }

    /////////////////////////////////////////
    element = element.findFirst(QStringLiteral("embed"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        return;
    }
    //もとに戻す
    foreach (const QString &key, embed.keys()) {
        element.evaluateJavaScript(QStringLiteral("this.%1='%2'").arg(key).arg(embed.value(key)));
    }

    /////////////////////////////////////////
    //解説とか
    element = frame->findFirstElement(QStringLiteral("#sectionWrap"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        return;
    }
    //もとに戻す
    foreach (const QString &key, sectionWrap.keys()) {
        element.setStyleProperty(key, sectionWrap.value(key));
    }

    /////////////////////////////////////////
    //友達招待のバルーンを移動させるためにサイズ調整
    element = frame->findFirstElement(QStringLiteral("#globalNavi"));
    element = element.findFirst(QStringLiteral("p"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        return;
    }
    //もとに戻す
    foreach (const QString &key, globalNavi.keys()) {
        element.setStyleProperty(key, globalNavi.value(key));
    }

}

WebView::WebView(QWidget *parent)
    : QWebView(parent)
    , d(new Private(this))
    , m_disableContextMenu(false)
{
    setAttribute(Qt::WA_AcceptTouchEvents, false);
    connect(this, &QObject::destroyed, [this]() { delete d; });

#if 0   //
    connect(this, &QWebView::loadFinished, [this](bool ok) {
        if(ok){
            qDebug() << "2 loadFinished " << url() << " game exist " << gameExists();
            QWebFrame *frame = page()->mainFrame();

            connect(frame->page(), &QWebPage::contentsChanged, [this](){
                qDebug() << "2 frame contentsChanged " << gameExists();
            });

            if(frame->childFrames().isEmpty()){
                qDebug() << "2 child frame empty";
                return;
            }
            frame = frame->childFrames().first();
            connect(frame, &QWebFrame::loadFinished, [this](bool ok){
                qDebug() << "3 frame loadFinished " << ok << " game exist " << gameExists();
            });
            connect(frame->page(), &QWebPage::contentsChanged, [this](){
                qDebug() << "3 frame contentsChanged " << gameExists();
            });
            connect(frame->page(), &QWebPage::loadFinished, [this](bool ok){
                qDebug() << " frame loadFinished " << gameExists();
            });

            QWebElement element = frame->findFirstElement(QStringLiteral("#flashWrap"));
            if (element.isNull()) {
                qDebug() << "none flashWrap";
                return;
            }
            qDebug() << "flashWrap : " << element.toPlainText();
        }
    });
    connect(page(), &QWebPage::contentsChanged, [this](){
        qDebug() << "1 contentsChanged " << gameExists();
    });

    connect(page(), &QWebPage::frameCreated, [this](QWebFrame *frame){
        qDebug() << "1 frameCreated " << frame->url() << "," << frame->childFrames().length();
        connect(frame, &QWebFrame::loadFinished, [this](bool ok){
            qDebug() << "1 loadFinished " << ok << " game exist " << gameExists();
        });

        if(frame->childFrames().isEmpty()){
            qDebug() << "1 child frame empty";
            return;
        }
        frame = frame->childFrames().first();
        connect(frame, &QWebFrame::loadFinished, [this](bool ok){
            qDebug() << "1 frame loadFinished " << ok << " game exist " << gameExists();
        });
        connect(frame->page(), &QWebPage::contentsChanged, [this](){
            qDebug() << "1 frame contentsChanged " << gameExists();
        });
    });
#endif
}

bool WebView::gameExists() const
{
    return getGameRect().isValid();
}

//ゲームの領域を調べる
QRect WebView::getGameRect() const
{
    //フレームを取得
    QWebFrame *frame = page()->mainFrame();
    if (frame->childFrames().isEmpty()) {
        return QRect();
    }
    //フレームの子供からflashの入ったdivを探す。さらにその中のembedタグを調べる
    frame = d->searchGameFrame(frame);
    QWebElement element = frame->findFirstElement(QStringLiteral("#flashWrap"));
    if (element.isNull()) {
        qDebug() << "not found #flashWrap";
        return QRect();
    }
    element = element.findFirst(QStringLiteral("embed"));
    if (element.isNull()) {
        qDebug() << "not found embed";
        return QRect();
    }
    //見つけたタグの座標を取得
    QRect geometry = element.geometry();
    geometry.moveTopLeft(geometry.topLeft() + frame->geometry().topLeft());
    qDebug() << "flash geo:" << geometry;

    return geometry;
}

//ゲーム画面をキャプチャ
QImage WebView::capture(bool adjustScrollPosition)
{
    QImage ret;
    QImage temp;

    //スクロール位置の保存
    QPoint currentPos = page()->mainFrame()->scrollPosition();
    //表示位置を一番上へ強制移動
    if(adjustScrollPosition){
        page()->mainFrame()->setScrollPosition(QPoint(0, 0));
    }

    QRect geometry = getGameRect();
    if (!geometry.isValid()) {
        emit error(tr("failed find target"));
        goto finally;
    }

    if(!adjustScrollPosition){
        geometry.moveTo(geometry.x() - currentPos.x(), geometry.y() - currentPos.y());
    }

    {
        QRect image_geo(geometry);
        if(d->defaultRect.isValid()){
            if(image_geo.width() != d->defaultRect.width() || image_geo.height() != d->defaultRect.height()){
                image_geo.setWidth(d->defaultRect.width());
                image_geo.setHeight(d->defaultRect.height());
            }
        }

        ret = QImage(image_geo.size(), QImage::Format_ARGB32);  //最終形
        temp = QImage(geometry.size(), QImage::Format_ARGB32);  //Webviewのコピー

        QPainter painter(&ret);
        QPainter painterTemp(&temp);

        if(image_geo.width() != geometry.width() || image_geo.height() != geometry.height()){
            //テンポラリに全体を描画
            render(&painterTemp, QPoint(0,0), geometry);
            painter.drawImage(0, 0, temp.scaled(image_geo.width(), image_geo.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }else{
            //そのまま本番に描画
            render(&painter, QPoint(0,0), geometry);
        }
    }

finally:
    //スクロールの位置を戻す
    if(adjustScrollPosition){
        page()->mainFrame()->setScrollPosition(currentPos);
    }
    return ret;
}

WebView::ViewMode WebView::viewMode() const
{
    return d->viewMode;
}

void WebView::setViewMode(ViewMode viewMode)
{
    if (d->viewMode == viewMode) return;
    d->viewMode = viewMode;
    emit viewModeChanged(viewMode);
}
//リンクをクリックした時のメニューをつくる
void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());

    //背景をクリック
    if(!r.boundingRect().isEmpty()){
        //右クリックメニュー無効
        if(disableContextMenu()){
            return;
        }
    }
    //リンクをクリック
    if(!r.linkUrl().isEmpty()){
        QMenu menu(this);
        menu.addAction(tr("Open in New Tab"), this, SLOT(openLinkInNewTab()));
        menu.addSeparator();
        menu.addAction(pageAction(QWebPage::CopyLinkToClipboard));
        menu.exec(mapToGlobal(event->pos()));
        return;
    }
    QWebView::contextMenuEvent(event);
}
//WebViewへのクリックイベント
void WebView::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(event);
    QWebView::mousePressEvent(event);
}
//タブで開くのトリガー
void WebView::openLinkInNewTab()
{
    pageAction(QWebPage::OpenLinkInNewWindow)->trigger();
}

//ゲームの画面サイズ
qreal WebView::getGameSizeFactor() const
{
    return gameSizeFactor;
}
//ゲームの画面サイズ
void WebView::setGameSizeFactor(const qreal &factor)
{
    if(gameSizeFactor == factor) return;

    if(factor < 0){
        gameSizeFactor = 0;
    }else{
        gameSizeFactor = factor;
    }
    emit gameSizeFactorChanged(factor);
}
//右クリックメニューを無効化する
bool WebView::disableContextMenu() const
{
    return m_disableContextMenu;
}
void WebView::setDisableContextMenu(bool disableContextMenu)
{
    m_disableContextMenu = disableContextMenu;
}
