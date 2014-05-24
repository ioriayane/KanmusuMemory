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
#ifndef FAVORITEMENU_H
#define FAVORITEMENU_H

#include <QObject>
#include <QMenu>
#include <QDate>

class FavoriteMenu : public QObject
{
    Q_OBJECT
public:
    explicit FavoriteMenu(QObject *parent);

    void load(QMenu *menu, bool download = false);

signals:
    void selectFav(const QUrl &url);
    void downloadFinished();

public slots:
    void clickItem();
    void updateFromInternet(const QString &lastUpdateDate);

private:
    QDate m_currentLoadedFavDataDate;   //現在読み込んでるお気に入りの作成日

    bool addItem(QMenu *parent, const QJsonArray &array);
};

#endif // FAVORITE_H
