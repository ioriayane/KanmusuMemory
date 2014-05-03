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
    bool isContainMajorDamageShip() const;
    bool checkTurnCompassScreen() const;

private:
    void detectScreenType();
    void setScreenType(GameScreen::ScreenType s);
    QRgb color(const QRect &rect) const;
    QRgb color(const QImage &img, const QRect &rect) const;
    bool fuzzyCompare(QRgb a, QRgb b, int range = 0x20) const;
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
//表示してる画面を判定
void GameScreen::Private::detectScreenType()
{
    if (fuzzyCompare(color(QRect(0, 0, 5, 5)), qRgb(45, 43, 43))) {
        //カタログ画面か
        setScreenType(CatalogScreen);

    }else if(fuzzyCompare(color(BUTTLE_RESULT_RECT1), BUTTLE_RESULT_CHECK_COLOR1)
             && fuzzyCompare(color(BUTTLE_RESULT_RECT2), BUTTLE_RESULT_CHECK_COLOR2)) {
        //戦果画面
        setScreenType(ButtleResultScreen);

#if 0
    }else if(fuzzyCompare(color(BUTTLE_RECT1), BUTTLE_DAYTIME_CHECK_COLOR1, 10)
             && fuzzyCompare(color(BUTTLE_RECT2), BUTTLE_DAYTIME_CHECK_COLOR2)) {
        //昼戦
//        QRgb rgb1 = color(BUTTLE_RECT1);
//        QRgb rgb2 = color(BUTTLE_RECT2);
//        qDebug() << "daytime:" << qRed(rgb1) << "," << qGreen(rgb1) << "," << qBlue(rgb1)
//                     << qRed(rgb2) << "," << qGreen(rgb2) << "," << qBlue(rgb2);
        setScreenType(DaytimeButtleScreen);
    }else if(fuzzyCompare(color(BUTTLE_RECT1), BUTTLE_NIGHT_CHECK_COLOR1, 10)
             && fuzzyCompare(color(BUTTLE_RECT2), BUTTLE_NIGHT_CHECK_COLOR2)) {
        //夜戦
//        QRgb rgb1 = color(BUTTLE_RECT1);
//        QRgb rgb2 = color(BUTTLE_RECT2);
//        qDebug() << "night:" << qRed(rgb1) << "," << qGreen(rgb1) << "," << qBlue(rgb1)
//                     << qRed(rgb2) << "," << qGreen(rgb2) << "," << qBlue(rgb2);
        setScreenType(NightButtleScreen);
#endif
    }else if(fuzzyCompare(color(BUTTLE_GO_OR_BACK_RECT), BUTTLE_GO_OR_BACK_CHECK_COLOR)){
        //戦闘後（進撃・撤退）選択画面
        setScreenType(GoOrBackScreen);

    }else if(checkTurnCompassScreen()){
        //羅針盤を回す画面
        // 他の画面も誤認してしまってるので注意
        setScreenType(TurnCompassScreen);
    }
}
//画面の特定部分を表示してるかを判定
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
//戦果報告画面で大破が含まれるか
bool GameScreen::Private::isContainMajorDamageShip() const
{
    bool ret = false;
    if(screenType == GameScreen::ButtleResultScreen){
//        qDebug() << " Buttle Result";
        int y[] = {189, 234, 279, 324, 369, 414};

        QImage mask_image(":/resources/MajorDamageMask.png");
        QImage imagework(mask_image);
        QPainter painter(&imagework);
        for(int yi=0; yi<6; yi++){
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            painter.drawImage(0, 0, mask_image);
            painter.setCompositionMode(QPainter::RasterOp_SourceAndDestination);
            painter.drawImage(0, 0, image, 196, y[yi]);

//            QRgb rgb = color(imagework, BUTTLE_RESULT_MAJOR_DAMAGE_RECT);
//            qDebug() << yi << "  color:" << qRed(rgb) << "," << qGreen(rgb) << "," << qBlue(rgb)
//                     << ":" << fuzzyCompare(color(imagework, BUTTLE_RESULT_MAJOR_DAMAGE_RECT), BUTTLE_RESULT_MAJOR_DAMAGE_CHECK_COLOR, 5);

            ret |= fuzzyCompare(color(imagework, BUTTLE_RESULT_MAJOR_DAMAGE_RECT), BUTTLE_RESULT_MAJOR_DAMAGE_CHECK_COLOR, 5);
        }
    }
    return ret;
}
//コンパスを回す画面化調べる
bool GameScreen::Private::checkTurnCompassScreen() const
{
    QImage imagework(":/resources/CompassMask.png");
    QPainter painter(&imagework);
    painter.setCompositionMode(QPainter::RasterOp_SourceAndDestination);
    painter.drawImage(0, 0, image, BUTTLE_COMPASS_RECT.x(), BUTTLE_COMPASS_RECT.y());

//    QRgb rgb = color(imagework, QRect(0, 0, imagework.width(), imagework.height()));
//    qDebug() << "compass color:" << qRed(rgb) << "," << qGreen(rgb) << "," << qBlue(rgb)
//             << ":" << fuzzyCompare(color(imagework, QRect(0, 0, imagework.width(), imagework.height())), BUTTLE_COMPASS_CHECK_COLOR, 5);
    return fuzzyCompare(color(imagework, QRect(0, 0, imagework.width(), imagework.height())), BUTTLE_COMPASS_CHECK_COLOR, 5);
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

QRgb GameScreen::Private::color(const QImage &img, const QRect &rect) const
{
    return img.copy(rect).scaled(1, 1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).pixel(0, 0);
}

bool GameScreen::Private::fuzzyCompare(QRgb a, QRgb b, int range) const
{
//    qDebug() << Q_FUNC_INFO << __LINE__ << qRed(a) << qGreen(a) << qBlue(a);
//    qDebug() << Q_FUNC_INFO << __LINE__ << qRed(b) << qGreen(b) << qBlue(b);
    return (qAbs(qRed(a) - qRed(b)) < range) && (qAbs(qGreen(a) - qGreen(b)) < range) && (qAbs(qBlue(a) - qBlue(b)) < range);
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
//戦果報告画面で大破が含まれるか
bool GameScreen::isContainMajorDamageShip() const
{
    return d->isContainMajorDamageShip();
}

void GameScreen::click(WebView *webView, GameScreen::PartType partType, GameScreen::WaitInterval waitInterval)
{
    d->click(webView, partType, waitInterval);
}

