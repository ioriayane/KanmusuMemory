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
#include "kanmusumemory_global.h"

#include <QSettings>
#include <QDebug>

class TabWidget::Private
{
public:
    Private(TabWidget *parent);

    void newTab(const QUrl &url);
    void newTab(QWebPage *webpage);

private:
    TabWidget *q;

};

TabWidget::Private::Private(TabWidget *parent)
    : q(parent)
{

}
//新しいタブ
void TabWidget::Private::newTab(const QUrl &url)
{
    qDebug() << "newTab:" << url;

    WebPageForm *web = new WebPageForm(q);
    web->setUrl(url);
    q->addTab(web, QStringLiteral("(untitled)"));
    q->setCurrentWidget(web);

    //タブの追加
    connect(web, &WebPageForm::addTabRequested, [this](QWebPage *webpage){
        newTab(webpage);
    });
    //タブを閉じる
    connect(web, &WebPageForm::removeTabRequested, [this](WebPageForm *form) {
        q->closeTab(form);
    });
}

void TabWidget::Private::newTab(QWebPage *webpage)
{
    qDebug() << "newTab: webpage";

    WebPageForm *web = new WebPageForm(q);
    web->setWebPage(webpage);
    q->addTab(web, QStringLiteral("(untitled)"));
    q->setCurrentWidget(web);

    //タブの追加
    connect(web, &WebPageForm::addTabRequested, [this](QWebPage *webpage){
        newTab(webpage);
    });
    //タブを閉じる
    connect(web, &WebPageForm::removeTabRequested, [this](WebPageForm *form) {
        qDebug() << "removeTabRequest";
        q->closeTab(form);
    });
}

TabWidget::TabWidget(QWidget* parent)
    : QTabWidget(parent)
    , d(new Private(this))
    , m_saveOpenPage(true)
    , m_openAndNewTab(true)
{
    connect(this, &QObject::destroyed, [this]() { delete d; });

    //タブの閉じる要求
    connect(this, &QTabWidget::tabCloseRequested, [this](int index) {
        WebPageForm *form = reinterpret_cast<WebPageForm *>(widget(index));
        qDebug() << "close tab(" << index << "):" << form->url();
        delete form;
    });

    //タブ復元
    load();
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
void TabWidget::openUrl(const QUrl &url)
{
    if(openAndNewTab() || count() == 0){
        //設定が新規タブとタブが無いとき
        newTab(url);
    }else{
        //現在のタブ
        WebPageForm *form = reinterpret_cast<WebPageForm *>(widget(currentIndex()));
        form->setUrl(url);
    }
}
//新しいタブ
void TabWidget::newTab(const QUrl &url)
{
    d->newTab(url);
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

//タブとかを読み込む
void TabWidget::load()
{
    QSettings settings(SETTING_FILE_NAME, SETTING_FILE_FORMAT);
    settings.beginGroup(QStringLiteral(SETTING_TAB));
    setSaveOpenPage(settings.value(QStringLiteral(SETTING_TAB_SAVE_OPEN_PAGES), true).toBool());
    setOpenAndNewTab(settings.value(QStringLiteral(SETTING_TAB_OPEN_AND_NEWTAB), true).toBool());
    if(count() == 0 && saveOpenPage()){   //タブが無いときだけ復元
        QList<QVariant> pagelist = settings.value(QStringLiteral(SETTING_TAB_OPEN_PAGES), QVariant()).toList();
        foreach (QVariant page, pagelist) {
            newTab(page.toString());
        }
    }
    settings.endGroup();
}
//タブとか設定を保存する
void TabWidget::save()
{
    QList<QVariant> pagelist;
    WebPageForm *form;
    QSettings settings(SETTING_FILE_NAME, SETTING_FILE_FORMAT);
    settings.beginGroup(QStringLiteral(SETTING_TAB));
    settings.setValue(QStringLiteral(SETTING_TAB_SAVE_OPEN_PAGES), saveOpenPage());
    settings.setValue(QStringLiteral(SETTING_TAB_OPEN_AND_NEWTAB), openAndNewTab());
    if(saveOpenPage()){
        for(int i=0; i<count(); i++){
            form = reinterpret_cast<WebPageForm *>(widget(i));
            pagelist.append(form->url().toString());
        }
    }
    settings.setValue(QStringLiteral(SETTING_TAB_OPEN_PAGES), pagelist);
    settings.endGroup();
}

