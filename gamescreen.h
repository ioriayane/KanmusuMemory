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
#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include <QtCore/QObject>

class WebView;

class GameScreen : public QObject
{
    Q_OBJECT
    Q_ENUMS(ScreenType)
    Q_PROPERTY(ScreenType screenType READ screenType NOTIFY screenTypeChanged)
public:
    enum ScreenType {
        UnknownScreen,
        BaseScreen,
        CatalogScreen,
        ItemScreen,
        SortieScreen,
        FleetScreen,
        CustomizeScreen,
        SupplyScreen,
        RepairScreen,
        FactoryScreen,
        DaytimeButtleScreen,        //昼間戦闘画面
        NightButtleScreen,          //夜戦画面
        ButtleResultScreen,         //戦果画面
        GoOrBackScreen,             //進撃or撤退画面
        TurnCompassScreen,          //羅針盤を回す画面
        ExpeditionScreen            //遠征選択画面
    };

    enum PartType {
        HeaderPart,
        Ship1Part,
        Ship2Part,
        Ship3Part,
        Ship4Part,
        Ship5Part,
        Ship6Part
    };

    enum WaitInterval {
        WaitNormal = 500,
        WaitLonger = 1500
    };

    explicit GameScreen(const QImage &image, QObject *parent = 0);
    ScreenType screenType() const;
    bool isVisible(PartType partType) const;
    bool isContainMajorDamageShip() const;

    int getClickExpeditionItemFleetNo(const QPointF &pos) const;
    void getExpeditionTime(qint64 *total, qint64 *remain);
    int getExpeditionFleetNo() const;
    bool isClickExpeditionStartButton(const QPointF &pos) const;

public slots:
    void click(WebView *webView, PartType partType, WaitInterval waitInterval = WaitNormal);

signals:
    void screenTypeChanged(ScreenType screenType);

private:
    class Private;
    Private *d;
    Q_DISABLE_COPY(GameScreen)
};

#endif // GAMESCREEN_H
