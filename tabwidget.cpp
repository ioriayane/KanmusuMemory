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
#include "tabwidget.h"
#include "webpage.h"
#include "kanmusumemory_global.h"

#include <QSettings>
#include <QDebug>

class TabWidget::Private
{
public:
    Private(TabWidget *parent);

    void newTab(const QUrl &url, bool mobilemode = true);
    void newTab(WebPage *webpage, bool mobilemode = true);

private:
    TabWidget *q;

};

TabWidget::Private::Private(TabWidget *parent)
    : q(parent)
{

}
//新しいタブ
void TabWidget::Private::newTab(const QUrl &url, bool mobilemode)
{
//    qDebug() << "newTab:" << url << " : mobile=" << mobilemode;

    WebPageForm *web = new WebPageForm(q);
    web->setMobileMode(mobilemode);
    web->setCache(q->cache());
    web->setUrl(url);
    q->addTab(web, QStringLiteral("(untitled)"));
    q->setCurrentWidget(web);

    //タブの追加
    connect(web, &WebPageForm::addTabRequested, [this](WebPage *webpage){
        newTab(webpage);
    });
    //タブを閉じる
    connect(web, &WebPageForm::removeTabRequested, [this](WebPageForm *form) {
        q->closeTab(form);
    });
    //お気に入りの更新
    connect(web, &WebPageForm::updateFavorite, [this]() { emit q->updateFavorite(); });
}

void TabWidget::Private::newTab(WebPage *webpage, bool mobilemode)
{
//    qDebug() << "newTab: webpage";

    WebPageForm *web = new WebPageForm(q);
    web->setMobileMode(mobilemode);
    web->setWebPage(webpage);
    web->setCache(q->cache());
    q->addTab(web, QStringLiteral("(untitled)"));
    q->setCurrentWidget(web);

    //タブの追加
    connect(web, &WebPageForm::addTabRequested, [this](WebPage *webpage){
        newTab(webpage);
    });
    //タブを閉じる
    connect(web, &WebPageForm::removeTabRequested, [this](WebPageForm *form) {
        q->closeTab(form);
    });
    //お気に入りの更新
    connect(web, &WebPageForm::updateFavorite, [this]() { emit q->updateFavorite(); });
}

TabWidget::TabWidget(QWidget* parent)
    : QTabWidget(parent)
    , d(new Private(this))
    , m_saveOpenPage(true)
    , m_openAndNewTab(true)
    , m_cache(NULL)
{
    connect(this, &QObject::destroyed, [this]() { delete d; });

    //タブの閉じる要求
    connect(this, &QTabWidget::tabCloseRequested, [this](int index) {
        WebPageForm *form = reinterpret_cast<WebPageForm *>(widget(index));
//        qDebug() << "close tab(" << index << "):" << form->url();
        delete form;
    });
}

//タブの状態を保存する
bool TabWidget::saveOpenPage() const
{
    return m_saveOpenPage;
}
void TabWidget::setSaveOpenPage(bool saveOpenPage)
{
    m_saveOpenPage = saveOpenPage;
}
//新しいURLを開くとき新しいタブ
bool TabWidget::openAndNewTab() const
{
    return m_openAndNewTab;
}
void TabWidget::setOpenAndNewTab(bool openAndNewTab)
{
    m_openAndNewTab = openAndNewTab;
}

//リンクを開く
void TabWidget::openUrl(const QUrl &url, bool mobilemode)
{
    if(openAndNewTab() || count() == 0){
        //設定が新規タブとタブが無いとき
        newTab(url, mobilemode);
    }else{
        //現在のタブ
        WebPageForm *form = reinterpret_cast<WebPageForm *>(widget(currentIndex()));
        form->setMobileMode(mobilemode);
        form->setUrl(url);
    }
}
//新しいタブ
void TabWidget::newTab(const QUrl &url, bool mobilemode)
{
    d->newTab(url, mobilemode);
}
//タブを閉じる
void TabWidget::closeTab()
{
    tabCloseRequested(currentIndex());
}
//タブを閉じる
void TabWidget::closeTab(WebPageForm *form)
{
    WebPageForm *f;
    for(int i=0; i<count(); i++){
        f = reinterpret_cast<WebPageForm *>(widget(i));
        if(f == form){
            tabCloseRequested(i);
        }
    }
}
//タブをすべて閉じる
void TabWidget::closeTabAll()
{
    for(int i = count(); i >= 0; i++){
        tabCloseRequested(i);
    }
}
//タブの再読み込み
void TabWidget::reloadTab()
{
    WebPageForm *form = reinterpret_cast<WebPageForm *>(currentWidget());
    form->reload();
}
//前のタブ
void TabWidget::prevTab()
{
    int index = currentIndex() - 1;
    if(index < 0){
        index = count() - 1;
    }
    setCurrentIndex(index);
}
//次のタブ
void TabWidget::nextTab()
{
    int index = currentIndex() + 1;
    if(index >= count()){
        index = 0;
    }
    setCurrentIndex(index);
}

void TabWidget::find()
{
    WebPageForm *form = qobject_cast<WebPageForm *>(currentWidget());
    if(form){
        form->find();
    }
}

//タブとかを読み込む
void TabWidget::load()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, KANMEMO_PROJECT, KANMEMO_NAME);
    settings.beginGroup(QStringLiteral(SETTING_TAB));
    setSaveOpenPage(settings.value(QStringLiteral(SETTING_TAB_SAVE_OPEN_PAGES), true).toBool());
    setOpenAndNewTab(settings.value(QStringLiteral(SETTING_TAB_OPEN_AND_NEWTAB), true).toBool());
    if(count() == 0 && saveOpenPage()){   //タブが無いときだけ復元
        QList<QVariant> pagelist = settings.value(QStringLiteral(SETTING_TAB_OPEN_PAGES), QVariant()).toList();
        QList<QVariant> pageMobileModeList = settings.value(QStringLiteral(SETTING_TAB_OPEN_PAGES_MOBILE_MODE), QVariant()).toList();
        int i=0;
        foreach (QVariant page, pagelist) {
            if(i < pageMobileModeList.count()){
                newTab(page.toString(), pageMobileModeList[i].toBool());
                i++;
            }else{
                newTab(page.toString());
            }
        }
    }
    settings.endGroup();
}
//タブとか設定を保存する
void TabWidget::save()
{
    QList<QVariant> pagelist;
    QList<QVariant> pageMobileModeList;
    WebPageForm *form;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, KANMEMO_PROJECT, KANMEMO_NAME);
    settings.beginGroup(QStringLiteral(SETTING_TAB));
    settings.setValue(QStringLiteral(SETTING_TAB_SAVE_OPEN_PAGES), saveOpenPage());
    settings.setValue(QStringLiteral(SETTING_TAB_OPEN_AND_NEWTAB), openAndNewTab());
    if(saveOpenPage()){
        for(int i=0; i<count(); i++){
            form = reinterpret_cast<WebPageForm *>(widget(i));
            pagelist.append(form->url().toString());
            pageMobileModeList.append(form->isMobileMode());
        }
    }
    settings.setValue(QStringLiteral(SETTING_TAB_OPEN_PAGES), pagelist);
    settings.setValue(QStringLiteral(SETTING_TAB_OPEN_PAGES_MOBILE_MODE), pageMobileModeList);
    settings.endGroup();
}
//キャッシュをタブの中のWebページへ設定するための情報
QNetworkDiskCache *TabWidget::cache() const
{
    return m_cache;
}
void TabWidget::setCache(QNetworkDiskCache *cache)
{
    m_cache = cache;
}


