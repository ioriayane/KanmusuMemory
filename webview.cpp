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

class WebView::Private
{
public:
    Private(WebView *parent);

private:
    void showNormal();
    void showFullScreen();

private:
    WebView *q;

public:
    ViewMode viewMode;

private:
    QHash<QString, QString> body;
    QHash<QString, QString> gameFrame;
    QHash<QString, QString> flashWrap;
    QHash<QString, QString> embed;
    QHash<QString, QString> sectionWrap;
};

WebView::Private::Private(WebView *parent)
    : q(parent)
    , viewMode(NormalMode)
{
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
}

void WebView::Private::showFullScreen()
{
    //normal -> full

    QWebFrame *frame = q->page()->mainFrame();

    //スクロールバー非表示
    QWebElement element = frame->findFirstElement(QStringLiteral("body"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        return;
    }

    QHash<QString, QString> properties;
    properties.insert(QStringLiteral("overflow"), QStringLiteral("hidden"));
    if(body.isEmpty()){
        foreach (const QString &key, properties.keys()) {
            body.insert(key, element.styleProperty(key, QWebElement::InlineStyle));
        }
        qDebug() << element.styleProperty(QStringLiteral("overflow"), QWebElement::InlineStyle);
    }
    foreach (const QString &key, properties.keys()) {
        element.setStyleProperty(key, properties.value(key));
    }
    properties.clear();


    //フレームを最大化
    element = frame->findFirstElement(QStringLiteral("#game_frame"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return;
    }

    properties.insert(QStringLiteral("position"), QStringLiteral("absolute"));
    properties.insert(QStringLiteral("top"), QStringLiteral("0px"));
    properties.insert(QStringLiteral("left"), QStringLiteral("0px"));
    properties.insert(QStringLiteral("width"), QString("%1px").arg(q->window()->width()));
    properties.insert(QStringLiteral("height"), QString("%1px").arg(q->window()->height()));
    properties.insert(QStringLiteral("z-index"), QStringLiteral("10"));
    if(gameFrame.isEmpty()){
        foreach (const QString &key, properties.keys()) {
            gameFrame.insert(key, element.styleProperty(key, QWebElement::InlineStyle));
        }
        qDebug() << element.styleProperty(QStringLiteral("position"), QWebElement::InlineStyle)
                 << "," << element.styleProperty(QStringLiteral("top"), QWebElement::InlineStyle)
                 << "," << element.styleProperty(QStringLiteral("left"), QWebElement::InlineStyle)
                 << "," << element.styleProperty(QStringLiteral("width"), QWebElement::InlineStyle)
                 << "," << element.styleProperty(QStringLiteral("height"), QWebElement::InlineStyle);
    }
    foreach (const QString &key, properties.keys()) {
        element.setStyleProperty(key, properties.value(key));
    }
    properties.clear();

    //フレームの子供からflashの入ったdivを探して、さらにその中のembedタグを調べる
    frame = frame->childFrames().first();
    element = frame->findFirstElement(QStringLiteral("#flashWrap"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return;
    }

    properties.insert(QStringLiteral("position"), QStringLiteral("absolute"));
    properties.insert(QStringLiteral("top"), QStringLiteral("0px"));
    properties.insert(QStringLiteral("left"), QStringLiteral("0px"));
    properties.insert(QStringLiteral("width"), QString("%1px").arg(q->window()->width()));
    properties.insert(QStringLiteral("height"), QString("%1px").arg(q->window()->height()));
    if(flashWrap.isEmpty()){
        foreach (const QString &key, properties.keys()) {
            flashWrap.insert(key, element.styleProperty(key, QWebElement::InlineStyle));
        }
        qDebug() << element.styleProperty(QStringLiteral("position"), QWebElement::InlineStyle)
                 << "," << element.styleProperty(QStringLiteral("top"), QWebElement::InlineStyle)
                 << "," << element.styleProperty(QStringLiteral("left"), QWebElement::InlineStyle)
                 << "," << element.styleProperty(QStringLiteral("width"), QWebElement::InlineStyle)
                 << "," << element.styleProperty(QStringLiteral("height"), QWebElement::InlineStyle);
    }
    foreach (const QString &key, properties.keys()) {
        element.setStyleProperty(key, properties.value(key));
    }
    properties.clear();

    element = element.findFirst(QStringLiteral("embed"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return;
    }

    properties.insert(QStringLiteral("width"), QString::number(q->window()->width()));
    properties.insert(QStringLiteral("height"), QString::number(q->window()->height()));
    if(embed.isEmpty()){
        foreach (const QString &key, properties.keys()) {
            embed.insert(key, element.attribute(key));
        }
        qDebug() << element.attribute(QStringLiteral("width"))
                 << "," << element.attribute(QStringLiteral("height"))
                 << "->" << q->window()->width() << "," << q->window()->height();
    }
    foreach (const QString &key, properties.keys()) {
        element.evaluateJavaScript(QString("this.%1='%2'").arg(key).arg(properties.value(key)));
    }
    properties.clear();

    //解説とか消す
    element = frame->findFirstElement(QStringLiteral("#sectionWrap"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return;
    }
    properties.insert(QStringLiteral("visibility"), QStringLiteral("hidden"));
    if(sectionWrap.isEmpty()){
        foreach (const QString &key, properties.keys()) {
            sectionWrap.insert(key, element.styleProperty(key, QWebElement::InlineStyle));
        }
    }
    foreach (const QString &key, properties.keys()) {
        element.setStyleProperty(key, properties.value(key));
    }
}

void WebView::Private::showNormal()
{
    //full -> normal

    QWebFrame *frame = q->page()->mainFrame();

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

    element = frame->findFirstElement(QStringLiteral("#game_frame"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return;
    }
    //もとに戻す
    foreach (const QString &key, gameFrame.keys()) {
        element.setStyleProperty(key, gameFrame.value(key));
    }

    //フレームの子供からflashの入ったdivを探して、さらにその中のembedタグを調べる
    frame = frame->childFrames().first();
    element = frame->findFirstElement(QStringLiteral("#flashWrap"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return;
    }
    //もとに戻す
    foreach (const QString &key, flashWrap.keys()) {
        element.setStyleProperty(key, flashWrap.value(key));
    }

    element = element.findFirst(QStringLiteral("embed"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return;
    }
    //もとに戻す
    foreach (const QString &key, embed.keys()) {
        element.evaluateJavaScript(QStringLiteral("this.%1='%2'").arg(key).arg(embed.value(key)));
    }

    //解説とか消す
    element = frame->findFirstElement(QStringLiteral("#sectionWrap"));
    if (element.isNull()) {
        qDebug() << "failed find target";
        //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
        return;
    }
    //もとに戻す
    foreach (const QString &key, sectionWrap.keys()) {
        element.setStyleProperty(key, sectionWrap.value(key));
    }
}

WebView::WebView(QWidget *parent)
    : QWebView(parent)
    , d(new Private(this))
{
    connect(this, &QObject::destroyed, [this]() { delete d; });
}

bool WebView::gameExists() const
{
    return getGameRect().isValid();
}

//ゲームの領域を調べる
QRect WebView::getGameRect() const
{
    //スクロール位置は破壊される
    //表示位置を一番上へ強制移動
    page()->mainFrame()->setScrollPosition(QPoint(0, 0));
    //フレームを取得
    QWebFrame *frame = page()->mainFrame();
    if (frame->childFrames().isEmpty()) {
        return QRect();
    }
    //フレームの子供からflashの入ったdivを探して、さらにその中のembedタグを調べる
    frame = frame->childFrames().first();
    QWebElement element = frame->findFirstElement(QStringLiteral("#flashWrap"));
    if (element.isNull()) {
        return QRect();
    }
    element = element.findFirst(QStringLiteral("embed"));
    if (element.isNull()) {
        return QRect();
    }
    //見つけたタグの座標を取得
    QRect geometry = element.geometry();
    geometry.moveTopLeft(geometry.topLeft() + frame->geometry().topLeft());

    return geometry;
}

//ゲーム画面をキャプチャ
QImage WebView::capture()
{
    QImage ret;

    //スクロール位置の保存
    QPoint currentPos = page()->mainFrame()->scrollPosition();
    QRect geometry = getGameRect();
    if (!geometry.isValid()) {
        emit error(tr("failed find target"));
        goto finally;
    }

    {
        ret = QImage(geometry.size(), QImage::Format_ARGB32);
        QPainter painter(&ret);
        //全体を描画
        render(&painter, QPoint(0,0), geometry);
    }

finally:
    //スクロールの位置を戻す
    page()->mainFrame()->setScrollPosition(currentPos);
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
