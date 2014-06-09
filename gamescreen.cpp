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
#include "gamescreen.h"
#include "kanmusumemory_global.h"
#include "webview.h"
#include "numberguide.h"

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
    Private(const QImage &image, GameScreen *parent, RecognizeInfo *recognizeInfo);
    bool isVisible(PartType partType) const;
    void click(WebView *webView, GameScreen::PartType partType, GameScreen::WaitInterval waitInterval);
    bool isContainMajorDamageShip() const;
    bool checkTurnCompassScreen() const;

    int getClickExpeditionItemFleetNo(const QPointF &pos);
    void getExpeditionTime(qint64 *total, qint64 *remain);
    int getExpeditionFleetNo();
    bool isClickExpeditionStartButton(const QPointF &pos);

private:
    void detectScreenType();
    void setScreenType(GameScreen::ScreenType s);
    QRgb color(const QRect &rect) const;
    QRgb color(const QImage &img, const QRect &rect) const;
    bool fuzzyCompare(QRgb a, QRgb b, int range = 0x20) const;
    void click(WebView *webView, const QPoint &point, GameScreen::WaitInterval waitInterval);
    void imageBinarization(QImage *img, const QRect &rect, const int border, const QRgb hi, const QRgb lo);
    int numberMatching(const QImage &image, const QRect &rect, const QList<NumberGuide> &guideList, QSize guideSize);
private:
    GameScreen *q;
    QImage image;

    //認識に必要な情報を管理してくれるクラス
    RecognizeInfo *recognizeInfo;


public:
    GameScreen::ScreenType screenType;
};

GameScreen::Private::Private(const QImage &image, GameScreen *parent, RecognizeInfo *recogExpediInfo)
    : q(parent)
    , image(image)
    , screenType(UnknownScreen)
{
    //認識に必要な情報を管理してくれるクラス
    if(recogExpediInfo != NULL){
        //外から指定されたらそっちを使う
        this->recognizeInfo = recogExpediInfo;
    }else{
        //NULLなら
        this->recognizeInfo = new RecognizeInfo();
    }

    detectScreenType();
}
//表示してる画面を判定
void GameScreen::Private::detectScreenType()
{
    if (fuzzyCompare(color(QRect(0, 0, 5, 5)), qRgb(45, 43, 43))) {
        //カタログ画面か
        setScreenType(CatalogScreen);

    }else if(fuzzyCompare(color(BUTTLE_RESULT_RECT1), BUTTLE_RESULT_CHECK_COLOR1, 0x10)
             && fuzzyCompare(color(BUTTLE_RESULT_RECT2), BUTTLE_RESULT_CHECK_COLOR2, 0x10)) {
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

    }else if(isVisible(HeaderPart) && fuzzyCompare(color(EXPEDITION_RECT), EXPEDITION_CHECK_COLOR)){
        //遠征画面か
        setScreenType(ExpeditionScreen);
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
//遠征画面でクリックしたアイテムの出撃艦隊番号
int GameScreen::Private::getClickExpeditionItemFleetNo(const QPointF &pos)
{
    int ret = -1;
    if(screenType == ExpeditionScreen){
        for(int i=0; i<recognizeInfo->itemRectList().length(); i++){
            if(recognizeInfo->itemRectList().at(i).contains(pos.x(), pos.y())){
                QImage work(image);
                //2値化
                imageBinarization(&work, work.rect(), 200, qRgb(255,255,255), qRgb(0,0,0));
                //既に遠征に出ているマークあるか
                int click_item_number = numberMatching(work, recognizeInfo->flagRectList().at(i)
                                                       , recognizeInfo->flagGuideList(), QSize(23, 20));
                qDebug() << "  checkClickExpeditionItem:pos=" << i << ", fleet=" << click_item_number;
                ret = click_item_number;
                break;
            }
        }
    }
    return ret;
}
//遠征の時間を取得する
void GameScreen::Private::getExpeditionTime(qint64 *total, qint64 *remain)
{
    if(screenType == ExpeditionScreen){
        QImage work(image);
        //2値化
        imageBinarization(&work, work.rect(), 200, qRgb(255,255,255), qRgb(0,0,0));
        //各桁の値を取り出す
        int remain_value[6];
        int total_value[6];
        bool remain_error = false;
        bool total_error = false;

        for(int i=0; i<6; i++){
            remain_value[i] = numberMatching(work, recognizeInfo->remainTimeRectList().at(i)
                                             , recognizeInfo->numberGuideList(), QSize(9, 13));
            total_value[i] = numberMatching(work, recognizeInfo->totalTimeRectList().at(i)
                                            , recognizeInfo->numberGuideList(), QSize(9, 13));
        }
        qDebug() << "  getExpeditionTime:" << total_value[0] << total_value[1] << total_value[2] << total_value[3] << total_value[4] << total_value[5];
        qDebug() << "                   :" << remain_value[0] << remain_value[1] << remain_value[2] << remain_value[3] << remain_value[4] << remain_value[5];

        for(int i=0; i<6; i++){
            if(remain_value[i] == -1)   remain_error = true;
            if(total_value[i] == -1)    total_error = true;
        }
        if(remain_error){
            *remain = -1;
        }else{
            *remain = ((remain_value[0]*10+remain_value[1])*3600 + (remain_value[2]*10+remain_value[3])*60 + remain_value[4]*10+remain_value[5])*1000;
        }
        if(total_error){
            *total = -1;
        }else{
            *total = ((total_value[0]*10+total_value[1])*3600 + (total_value[2]*10+total_value[3])*60 + total_value[4]*10+total_value[5])*1000;
        }
    }
}
//遠征開始時に選択している艦隊番号を取得する
int GameScreen::Private::getExpeditionFleetNo()
{
    int ret = -1;
    QImage work(image);
    //2値化
    imageBinarization(&work, EXPEDITION_SELECT_FLEET_RECT, 100, qRgb(255,255,255), qRgb(0,0,0));
    //旗の位置の色を取得
    QRgb rgb2 = color(work, QRect(381, 108, 23, 20));
    QRgb rgb3 = color(work, QRect(411, 108, 23, 20));
    QRgb rgb4 = color(work, QRect(441, 108, 23, 20));
    if(fuzzyCompare(rgb2, qRgb(255,255,255), 5)){
        ret = 2;
    }else if(fuzzyCompare(rgb3, qRgb(255,255,255), 5)){
        ret = 3;
    }else if(fuzzyCompare(rgb4, qRgb(255,255,255), 5)){
        ret = 4;
    }
    return ret;
}
//遠征開始ボタンを押したか？
bool GameScreen::Private::isClickExpeditionStartButton(const QPointF &pos)
{
    if(EXPEDITION_START_BUTTON_RECT.contains(pos.x(), pos.y())){
        return fuzzyCompare(color(EXPEDITION_START_BUTTON_RECT), EXPEDITION_START_BUTTON_COLOR);
    }else{
        return false;
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
//画像を二値化する
void GameScreen::Private::imageBinarization(QImage *img, const QRect &rect, const int border, const QRgb hi, const QRgb lo)
{
    int y0 = rect.y();
    int x0 = rect.x();
    int h = rect.height();
    int w = rect.width();

    for (int y = 0; y < h; y++) {
        QRgb *row = reinterpret_cast<QRgb *>(img->scanLine(y0 + y)) + x0;
        for (int x = 0; x < w; x++) {
            if((qRed(*row) > border) || (qGreen(*row) > border)|| (qBlue(*row) > border)){
                (*row) = hi;
            }else{
                (*row) = lo;
            }
            row++;
        }
    }
}
//数字とのマッチングする（2値化した画像を使用すること）
int GameScreen::Private::numberMatching(const QImage &image, const QRect &rect, const QList<NumberGuide> &guideList, QSize guideSize)
{
    QImage work(guideSize, QImage::Format_ARGB32);
    QPainter painter(&work);
    QRgb rgb;
    QRgb rgb_src;
    int ret = -1;
    int match_count = 0;

    qDebug() << "matching:" << rect;

    for(int m=0; m<guideList.length(); m++){
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(0, 0, image, rect.x(), rect.y(), rect.width(), rect.height());
        rgb_src = color(work, work.rect());

        painter.setCompositionMode(QPainter::RasterOp_SourceAndDestination);
        painter.drawImage(0, 0, guideList.at(m).image);
        rgb = color(work, work.rect());

        if(qRed(rgb_src) == 0 && qGreen(rgb_src) == 0 && qBlue(rgb_src) == 0){
            //元が真っ黒
            qDebug() << "  makuro";
            break;
        }else if(qRed(rgb) < guideList.at(m).border){
            qDebug() <<  QString("  %1:match(%2<%3)").arg(guideList.at(m).number).arg(qRed(rgb)).arg(guideList.at(m).border);
            ret = guideList.at(m).number;
            match_count++;
            break;
        }else{
            qDebug() <<  QString("  %1:(%2) %3").arg(guideList.at(m).number).arg(qRed(rgb)).arg(guideList.at(m).image.isNull());
        }
    }

    return ret;
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

GameScreen::GameScreen(const QImage &image, RecognizeInfo *recogExpediInfo, QObject *parent)
    : QObject(parent)
    , d(new Private(image, this, recogExpediInfo))
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
//遠征画面でどのアイテムをクリックしたか
int GameScreen::getClickExpeditionItemFleetNo(const QPointF &pos) const
{
    return d->getClickExpeditionItemFleetNo(pos);
}
//遠征の時間を取得する
void GameScreen::getExpeditionTime(qint64 *total, qint64 *remain)
{
    d->getExpeditionTime(total, remain);
}
//遠征開始時に選択してる艦隊番号取得する
int GameScreen::getExpeditionFleetNo() const
{
    return d->getExpeditionFleetNo();
}
//遠征開始ボタンを押したか？
bool GameScreen::isClickExpeditionStartButton(const QPointF &pos) const
{
    return d->isClickExpeditionStartButton(pos);
}

void GameScreen::click(WebView *webView, GameScreen::PartType partType, GameScreen::WaitInterval waitInterval)
{
    d->click(webView, partType, waitInterval);
}

