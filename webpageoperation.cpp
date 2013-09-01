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
#include "webpageoperation.h"

#include <QWebFrame>
#include <QWebElement>
#include <QDebug>

WebPageOperation::WebPageOperation(QWidget *parent) :
    QWidget(parent)
  ,m_webView(NULL)
  ,q(parent)
{
}

bool WebPageOperation::existGame()
{
    return getGameRect().isValid();
}

//ゲームの領域を調べる
QRect WebPageOperation::getGameRect()
{
    if(m_webView == NULL)   return QRect();

    //スクロール位置は破壊される
    //表示位置を一番上へ強制移動
    m_webView->page()->mainFrame()->setScrollPosition(QPoint(0, 0));
    //フレームを取得
    QWebFrame *frame = m_webView->page()->mainFrame();
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

void WebPageOperation::fullScreen(bool isFull)
{
    if(m_webView == NULL)
        return;

    if(isFull){
        //normal -> full

        QWebFrame *frame = m_webView->page()->mainFrame();

        //スクロールバー非表示
        QWebElement element = frame->findFirstElement(QStringLiteral("body"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            return;
        }
        if(m_body.isEmpty()){
            m_body.insert(QStringLiteral("overflow"), element.styleProperty(QStringLiteral("overflow"), QWebElement::InlineStyle));
            qDebug() << element.styleProperty(QStringLiteral("overflow"), QWebElement::InlineStyle);
        }
        element.setStyleProperty(QStringLiteral("overflow"),QStringLiteral("hidden"));



        //フレームを最大化
        element = frame->findFirstElement(QStringLiteral("#game_frame"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        if(m_gameFrame.isEmpty()){
            m_gameFrame.insert(QStringLiteral("position"), element.styleProperty(QStringLiteral("position"), QWebElement::InlineStyle));
            m_gameFrame.insert(QStringLiteral("top"), element.styleProperty(QStringLiteral("top"), QWebElement::InlineStyle));
            m_gameFrame.insert(QStringLiteral("left"), element.styleProperty(QStringLiteral("left"), QWebElement::InlineStyle));
            m_gameFrame.insert(QStringLiteral("width"), element.styleProperty(QStringLiteral("width"), QWebElement::InlineStyle));
            m_gameFrame.insert(QStringLiteral("height"), element.styleProperty(QStringLiteral("height"), QWebElement::InlineStyle));
            m_gameFrame.insert(QStringLiteral("z-index"), element.styleProperty(QStringLiteral("z-index"), QWebElement::InlineStyle));
            qDebug() << element.styleProperty(QStringLiteral("position"), QWebElement::InlineStyle)
                     << "," << element.styleProperty(QStringLiteral("top"), QWebElement::InlineStyle)
                     << "," << element.styleProperty(QStringLiteral("left"), QWebElement::InlineStyle)
                     << "," << element.styleProperty(QStringLiteral("width"), QWebElement::InlineStyle)
                     << "," << element.styleProperty(QStringLiteral("height"), QWebElement::InlineStyle);
        }
        element.setStyleProperty(QStringLiteral("position"),QStringLiteral("absolute"));
        element.setStyleProperty(QStringLiteral("top"),QStringLiteral("0px"));
        element.setStyleProperty(QStringLiteral("left"),QStringLiteral("0px"));
        element.setStyleProperty(QStringLiteral("width"),QString("%1px").arg(q->width()));
        element.setStyleProperty(QStringLiteral("height"),QString("%1px").arg(q->height()));
        element.setStyleProperty(QStringLiteral("z-index"),QStringLiteral("10"));



        //フレームの子供からflashの入ったdivを探して、さらにその中のembedタグを調べる
        frame = frame->childFrames().first();
        element = frame->findFirstElement(QStringLiteral("#flashWrap"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        if(m_flashWrap.isEmpty()){
            m_flashWrap.insert(QStringLiteral("position"), element.styleProperty(QStringLiteral("position"), QWebElement::InlineStyle));
            m_flashWrap.insert(QStringLiteral("top"), element.styleProperty(QStringLiteral("top"), QWebElement::InlineStyle));
            m_flashWrap.insert(QStringLiteral("left"), element.styleProperty(QStringLiteral("left"), QWebElement::InlineStyle));
            m_flashWrap.insert(QStringLiteral("width"), element.styleProperty(QStringLiteral("width"), QWebElement::InlineStyle));
            m_flashWrap.insert(QStringLiteral("height"), element.styleProperty(QStringLiteral("height"), QWebElement::InlineStyle));
            qDebug() << element.styleProperty(QStringLiteral("position"), QWebElement::InlineStyle)
                     << "," << element.styleProperty(QStringLiteral("top"), QWebElement::InlineStyle)
                     << "," << element.styleProperty(QStringLiteral("left"), QWebElement::InlineStyle)
                     << "," << element.styleProperty(QStringLiteral("width"), QWebElement::InlineStyle)
                     << "," << element.styleProperty(QStringLiteral("height"), QWebElement::InlineStyle);
        }
        element.setStyleProperty(QStringLiteral("position"),QStringLiteral("absolute"));
        element.setStyleProperty(QStringLiteral("top"),QStringLiteral("0px"));
        element.setStyleProperty(QStringLiteral("left"),QStringLiteral("0px"));
        element.setStyleProperty(QStringLiteral("width"),QString("%1px").arg(q->width()));
        element.setStyleProperty(QStringLiteral("height"),QString("%1px").arg(q->height()));

        element = element.findFirst(QStringLiteral("embed"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        if(m_embed.isEmpty()){
            m_embed.insert(QStringLiteral("width"), element.attribute(QStringLiteral("width")));
            m_embed.insert(QStringLiteral("height"), element.attribute(QStringLiteral("height")));
            qDebug() << element.attribute(QStringLiteral("width"))
                     << "," << element.attribute(QStringLiteral("height"))
                     << "->" << q->width() << "," << q->height();
        }
        element.evaluateJavaScript(QString("this.width='%1'").arg(q->width()));
        element.evaluateJavaScript(QString("this.height='%1'").arg(q->height()));

        //解説とか消す
        element = frame->findFirstElement(QStringLiteral("#sectionWrap"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        if(m_sectionWrap.isEmpty()){
            m_sectionWrap.insert(QStringLiteral("visibility"), element.styleProperty(QStringLiteral("visibility"), QWebElement::InlineStyle));
        }
        element.setStyleProperty(QStringLiteral("visibility"),QStringLiteral("hidden"));

    }else{
        //full -> normal

        QWebFrame *frame = m_webView->page()->mainFrame();

        //スクロールバー表示
        QWebElement element = frame->findFirstElement(QStringLiteral("body"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            return;
        }
        //もとに戻す
        foreach (const QString &key, m_body.keys()) {
            element.setStyleProperty(key, m_body.value(key));
        }

        element = frame->findFirstElement(QStringLiteral("#game_frame"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        //もとに戻す
        foreach (const QString &key, m_gameFrame.keys()) {
            element.setStyleProperty(key, m_gameFrame.value(key));
        }

        //フレームの子供からflashの入ったdivを探して、さらにその中のembedタグを調べる
        frame = frame->childFrames().first();
        element = frame->findFirstElement(QStringLiteral("#flashWrap"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        //もとに戻す
        foreach (const QString &key, m_flashWrap.keys()) {
            element.setStyleProperty(key, m_flashWrap.value(key));
        }

        element = element.findFirst(QStringLiteral("embed"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        //もとに戻す
        foreach (const QString &key, m_embed.keys()) {
            element.evaluateJavaScript(QStringLiteral("this.%1='%2'").arg(key).arg(m_embed.value(key)));
        }

        //解説とか消す
        element = frame->findFirstElement(QStringLiteral("#sectionWrap"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        //もとに戻す
        foreach (const QString &key, m_sectionWrap) {
            element.setStyleProperty(key, m_sectionWrap.value(key));
        }
    }
}

void WebPageOperation::setWebView(QWebView *value)
{
    m_webView = value;
}
