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
  ,m_body(parent)
  ,m_gameFrame(parent)
  ,m_flashWrap(parent)
  ,m_embed(parent)
  ,m_sectionWrap(parent)
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
            m_body.append(QStringLiteral("overflow"), element.styleProperty(QStringLiteral("overflow"), QWebElement::InlineStyle));
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
            m_gameFrame.append(QStringLiteral("position"), element.styleProperty(QStringLiteral("position"), QWebElement::InlineStyle));
            m_gameFrame.append(QStringLiteral("top"), element.styleProperty(QStringLiteral("top"), QWebElement::InlineStyle));
            m_gameFrame.append(QStringLiteral("left"), element.styleProperty(QStringLiteral("left"), QWebElement::InlineStyle));
            m_gameFrame.append(QStringLiteral("width"), element.styleProperty(QStringLiteral("width"), QWebElement::InlineStyle));
            m_gameFrame.append(QStringLiteral("height"), element.styleProperty(QStringLiteral("height"), QWebElement::InlineStyle));
            m_gameFrame.append(QStringLiteral("z-index"), element.styleProperty(QStringLiteral("z-index"), QWebElement::InlineStyle));
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
            m_flashWrap.append(QStringLiteral("position"), element.styleProperty(QStringLiteral("position"), QWebElement::InlineStyle));
            m_flashWrap.append(QStringLiteral("top"), element.styleProperty(QStringLiteral("top"), QWebElement::InlineStyle));
            m_flashWrap.append(QStringLiteral("left"), element.styleProperty(QStringLiteral("left"), QWebElement::InlineStyle));
            m_flashWrap.append(QStringLiteral("width"), element.styleProperty(QStringLiteral("width"), QWebElement::InlineStyle));
            m_flashWrap.append(QStringLiteral("height"), element.styleProperty(QStringLiteral("height"), QWebElement::InlineStyle));
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
            m_embed.append(QStringLiteral("width"), element.attribute(QStringLiteral("width")));
            m_embed.append(QStringLiteral("height"), element.attribute(QStringLiteral("height")));
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
            m_sectionWrap.append(QStringLiteral("visibility"), element.styleProperty(QStringLiteral("visibility"), QWebElement::InlineStyle));
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
        for(int i=0; i<m_body.length(); i++){
            element.setStyleProperty(m_body.name(i),m_body.value(i));
        }

        element = frame->findFirstElement(QStringLiteral("#game_frame"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        //もとに戻す
        for(int i=0; i<m_gameFrame.length(); i++){
            element.setStyleProperty(m_gameFrame.name(i),m_gameFrame.value(i));
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
        for(int i=0; i<m_flashWrap.length(); i++){
            element.setStyleProperty(m_flashWrap.name(i),m_flashWrap.value(i));
        }

        element = element.findFirst(QStringLiteral("embed"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        //もとに戻す
        for(int i=0; i<m_embed.length(); i++){
            element.evaluateJavaScript(QString("this.%1='%2'").arg(m_embed.name(i)).arg(m_embed.value(i)));
        }

        //解説とか消す
        element = frame->findFirstElement(QStringLiteral("#sectionWrap"));
        if (element.isNull()) {
            qDebug() << tr("failed find target");
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        //もとに戻す
        for(int i=0; i<m_sectionWrap.length(); i++){
            element.setStyleProperty(m_sectionWrap.name(i),m_sectionWrap.value(i));
        }
    }
}

void WebPageOperation::setWebView(QWebView *value)
{
    m_webView = value;
}
