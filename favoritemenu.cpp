#include "favoritemenu.h"

#include <QDebug>

FavoriteMenu::FavoriteMenu(QObject *parent, QMenu *menu) :
    QObject(parent)
  , menu(menu)
{
}

void FavoriteMenu::load()
{
    QMenu *wiki = new QMenu(menu);
    wiki->setObjectName(tr("Kanmusu"));
    wiki->addAction(tr("aaaaa"), this, SLOT(clickItem));
//    menu->addAction(tr("bbbbb"), this, SLOT(clickItem));
}

void FavoriteMenu::clickItem()
{
    qDebug() << "clickItem";
}
