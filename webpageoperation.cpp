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

#include <QWebView>
#include <QWebFrame>
#include <QWebElement>
#include <QDebug>

WebPageOperation::WebPageOperation(QWebView *webView)
    : webView(webView)
{
}

bool WebPageOperation::gameExists() const
{
    return getGameRect().isValid();
}

//ゲームの領域を調べる
QRect WebPageOperation::getGameRect() const
{
    if(webView == NULL)   return QRect();

    //スクロール位置は破壊される
    //表示位置を一番上へ強制移動
    webView->page()->mainFrame()->setScrollPosition(QPoint(0, 0));
    //フレームを取得
    QWebFrame *frame = webView->page()->mainFrame();
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
    if(webView == NULL)
        return;

    if(isFull){
        //normal -> full

        QWebFrame *frame = webView->page()->mainFrame();

        //スクロールバー非表示
        QWebElement element = frame->findFirstElement(QStringLiteral("body"));
        if (element.isNull()) {
            qDebug() << "failed find target";
            return;
        }

        QHash<QString, QString> properties;
        properties.insert(QStringLiteral("overflow"), QStringLiteral("hidden"));
        if(m_body.isEmpty()){
            foreach (const QString &key, properties.keys()) {
                m_body.insert(key, element.styleProperty(key, QWebElement::InlineStyle));
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
        properties.insert(QStringLiteral("width"), QString("%1px").arg(webView->window()->width()));
        properties.insert(QStringLiteral("height"), QString("%1px").arg(webView->window()->height()));
        properties.insert(QStringLiteral("z-index"), QStringLiteral("10"));
        if(m_gameFrame.isEmpty()){
            foreach (const QString &key, properties.keys()) {
                m_gameFrame.insert(key, element.styleProperty(key, QWebElement::InlineStyle));
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
        properties.insert(QStringLiteral("width"), QString("%1px").arg(webView->window()->width()));
        properties.insert(QStringLiteral("height"), QString("%1px").arg(webView->window()->height()));
        if(m_flashWrap.isEmpty()){
            foreach (const QString &key, properties.keys()) {
                m_flashWrap.insert(key, element.styleProperty(key, QWebElement::InlineStyle));
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

        properties.insert(QStringLiteral("width"), QString::number(webView->window()->width()));
        properties.insert(QStringLiteral("height"), QString::number(webView->window()->height()));
        if(m_embed.isEmpty()){
            foreach (const QString &key, properties.keys()) {
                m_embed.insert(key, element.attribute(key));
            }
            qDebug() << element.attribute(QStringLiteral("width"))
                     << "," << element.attribute(QStringLiteral("height"))
                     << "->" << webView->window()->width() << "," << webView->window()->height();
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
        if(m_sectionWrap.isEmpty()){
            foreach (const QString &key, properties.keys()) {
                m_sectionWrap.insert(key, element.styleProperty(key, QWebElement::InlineStyle));
            }
        }
        foreach (const QString &key, properties.keys()) {
            element.setStyleProperty(key, properties.value(key));
        }

    }else{
        //full -> normal

        QWebFrame *frame = webView->page()->mainFrame();

        //スクロールバー表示
        QWebElement element = frame->findFirstElement(QStringLiteral("body"));
        if (element.isNull()) {
            qDebug() << "failed find target";
            return;
        }
        //もとに戻す
        foreach (const QString &key, m_body.keys()) {
            element.setStyleProperty(key, m_body.value(key));
        }

        element = frame->findFirstElement(QStringLiteral("#game_frame"));
        if (element.isNull()) {
            qDebug() << "failed find target";
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
            qDebug() << "failed find target";
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        //もとに戻す
        foreach (const QString &key, m_flashWrap.keys()) {
            element.setStyleProperty(key, m_flashWrap.value(key));
        }

        element = element.findFirst(QStringLiteral("embed"));
        if (element.isNull()) {
            qDebug() << "failed find target";
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
            qDebug() << "failed find target";
            //            ui.statusBar->showMessage(tr("failed find target"), STATUS_BAR_MSG_TIME);
            return;
        }
        //もとに戻す
        foreach (const QString &key, m_sectionWrap) {
            element.setStyleProperty(key, m_sectionWrap.value(key));
        }
    }
}
