#ifndef FAVORITEMENU_H
#define FAVORITEMENU_H

#include <QObject>
#include <QMenu>

class FavoriteMenu : public QObject
{
    Q_OBJECT
public:
    explicit FavoriteMenu(QObject *parent);
    ~FavoriteMenu();

    void load(QMenu *menu);
signals:

public slots:
    void clickItem();

private:
    bool addItem(QMenu *parent, QJsonArray *array);
};

#endif // FAVORITE_H
