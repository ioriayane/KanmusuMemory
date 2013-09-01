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

#include <QtCore/QRect>
#include <QtGui/QColor>
#include <QtGui/QImage>

class GameScreen::Private
{
public:
    Private(const QImage &image, GameScreen *parent);
    bool isVisible(PartType partType) const;

private:
    void detectScreenType();
    void setScreenType(GameScreen::ScreenType s);
    QRgb color(const QRect &rect) const;
    bool fuzzyCompare(QRgb a, QRgb b) const;

private:
    GameScreen *q;
    const QImage &image;
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
    }
    return ret;
}

QRgb GameScreen::Private::color(const QRect &rect) const
{
    return image.copy(rect).scaled(1, 1).pixel(0, 0);
}

bool GameScreen::Private::fuzzyCompare(QRgb a, QRgb b) const
{
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

