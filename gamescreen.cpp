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
#include "gamescreen.h"
#include "kanmusumemory_global.h"
#include "webview.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QRect>
#include <QtCore/QTime>
#include <QtCore/QThread>
#include <QtGui/QColor>
#include <QtGui/QImage>
#include <QtGui/QMouseEvent>

class GameScreen::Private
{
public:
    Private(const QImage &image, GameScreen *parent);
    bool isVisible(PartType partType) const;
    void click(WebView *webView, GameScreen::PartType partType, GameScreen::WaitInterval waitInterval);

private:
    void detectScreenType();
    void setScreenType(GameScreen::ScreenType s);
    QRgb color(const QRect &rect) const;
    bool fuzzyCompare(QRgb a, QRgb b) const;
    void click(WebView *webView, const QPoint &point, GameScreen::WaitInterval waitInterval);

private:
    GameScreen *q;
    QImage image;
public:
    GameScreen::ScreenType screenType;
};

GameScreen::Private::Private(const QImage &image, GameScreen *parent)
    : q(parent)
    , image(image)
    , screenType(UnknownScreen)
{
    detectScreenType();
}

void GameScreen::Private::detectScreenType()
{
    //カタログ画面か
    if (fuzzyCompare(color(QRect(0, 0, 5, 5)), qRgb(45, 43, 43))) {
        setScreenType(CatalogScreen);
        return;
    }
}

bool GameScreen::Private::isVisible(PartType partType) const
{
    bool ret = false;
    switch (partType) {
    case HeaderPart:
        ret = fuzzyCompare(color(QRect(300, 3, 490, 4)), qRgb(42, 150, 163));
        break;
    case Ship1Part:
        ret = fuzzyCompare(color(DETAIL_RECT_SHIP1_1), DETAIL_CHECK_COLOR1)
                && fuzzyCompare(color(DETAIL_RECT_SHIP1_2), DETAIL_CHECK_COLOR2);
        break;
    case Ship2Part:
        ret = fuzzyCompare(color(DETAIL_RECT_SHIP2_1), DETAIL_CHECK_COLOR1)
                && fuzzyCompare(color(DETAIL_RECT_SHIP2_2), DETAIL_CHECK_COLOR2);
        break;
    case Ship3Part:
        ret = fuzzyCompare(color(DETAIL_RECT_SHIP3_1), DETAIL_CHECK_COLOR1)
                && fuzzyCompare(color(DETAIL_RECT_SHIP3_2), DETAIL_CHECK_COLOR2);
        break;
    case Ship4Part:
        ret = fuzzyCompare(color(DETAIL_RECT_SHIP4_1), DETAIL_CHECK_COLOR1)
                && fuzzyCompare(color(DETAIL_RECT_SHIP4_2), DETAIL_CHECK_COLOR2);
        break;
    case Ship5Part:
        ret = fuzzyCompare(color(DETAIL_RECT_SHIP5_1), DETAIL_CHECK_COLOR1)
                && fuzzyCompare(color(DETAIL_RECT_SHIP5_2), DETAIL_CHECK_COLOR2);
        break;
    case Ship6Part:
        ret = fuzzyCompare(color(DETAIL_RECT_SHIP6_1), DETAIL_CHECK_COLOR1)
                && fuzzyCompare(color(DETAIL_RECT_SHIP6_2), DETAIL_CHECK_COLOR2);
        break;
    }
    return ret;
}

void GameScreen::Private::click(WebView *webView, GameScreen::PartType partType, GameScreen::WaitInterval waitInterval)
{
    switch (partType) {
    case Ship1Part:
        click(webView, DETAIL_RECT_SHIP1_1.center(), waitInterval);
        break;
    case Ship2Part:
        click(webView, DETAIL_RECT_SHIP2_1.center(), waitInterval);
        break;
    case Ship3Part:
        click(webView, DETAIL_RECT_SHIP3_1.center(), waitInterval);
        break;
    case Ship4Part:
        click(webView, DETAIL_RECT_SHIP4_1.center(), waitInterval);
        break;
    case Ship5Part:
        click(webView, DETAIL_RECT_SHIP5_1.center(), waitInterval);
        break;
    case Ship6Part:
        click(webView, DETAIL_RECT_SHIP6_1.center(), waitInterval);
        break;
    default:
        break;
    }
}

void GameScreen::Private::click(WebView *webView, const QPoint &point, GameScreen::WaitInterval waitInterval)
{
    qDebug() << Q_FUNC_INFO << __LINE__ << webView << point << waitInterval;
    QPointF pos = point + webView->getGameRect().topLeft();
    QMouseEvent eventPress(QEvent::MouseButtonPress
                            , pos
                            , Qt::LeftButton, 0, 0);
    QCoreApplication::sendEvent(webView, &eventPress);
    QMouseEvent eventRelease(QEvent::MouseButtonRelease
                             , pos
                              , Qt::LeftButton, 0, 0);
    QCoreApplication::sendEvent(webView, &eventRelease);

    int interval = waitInterval + static_cast<int>(qrand() % CLICK_EVENT_FLUCTUATION);

    QTime timer;
    timer.start();
    while (timer.elapsed() < interval) {
        QCoreApplication::processEvents();
        QThread::usleep(1000);
    }
    qDebug() << Q_FUNC_INFO << __LINE__ << timer.elapsed();
    qDebug() << Q_FUNC_INFO << __LINE__ << webView << point << waitInterval;
}

QRgb GameScreen::Private::color(const QRect &rect) const
{
    return image.copy(rect).scaled(1, 1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).pixel(0, 0);
}

bool GameScreen::Private::fuzzyCompare(QRgb a, QRgb b) const
{
//    qDebug() << Q_FUNC_INFO << __LINE__ << qRed(a) << qGreen(a) << qBlue(a);
//    qDebug() << Q_FUNC_INFO << __LINE__ << qRed(b) << qGreen(b) << qBlue(b);
    return qAbs(qRed(a) - qRed(b)) < 0x20 && qAbs(qGreen(a) - qGreen(b)) < 0x20 && qAbs(qBlue(a) - qBlue(b)) < 0x20;
}

void GameScreen::Private::setScreenType(GameScreen::ScreenType s)
{
    if (screenType == s) return;
    screenType = s;
    emit q->screenTypeChanged(s);
}

GameScreen::GameScreen(const QImage &image, QObject *parent)
    : QObject(parent)
    , d(new Private(image, this))
{
    connect(this, &QObject::destroyed, [this]() { delete d; });
}

GameScreen::ScreenType GameScreen::screenType() const
{
    return d->screenType;
}

bool GameScreen::isVisible(GameScreen::PartType partType) const
{
    return d->isVisible(partType);
}

void GameScreen::click(WebView *webView, GameScreen::PartType partType, GameScreen::WaitInterval waitInterval)
{
    d->click(webView, partType, waitInterval);
}

