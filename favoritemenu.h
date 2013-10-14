#ifndef FAVORITEMENU_H
#define FAVORITEMENU_H

#include <QObject>
#include <QMenu>

class FavoriteMenu : public QObject
{
    Q_OBJECT
public:
    explicit FavoriteMenu(QObject *parent, QMenu *menu);

    void load();
signals:

public slots:
    void clickItem();

private:
    QMenu *menu;
};

#endif // FAVORITE_H
