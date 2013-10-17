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

private:
    QDate m_currentLoadedFavDataDate;   //現在読み込んでるお気に入りの作成日

    bool addItem(QMenu *parent, QJsonArray *array);
};

#endif // FAVORITE_H
