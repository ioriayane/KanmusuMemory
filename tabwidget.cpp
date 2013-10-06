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
{
    connect(this, &QObject::destroyed, [this]() { delete d; });

    //タブの閉じる要求
    connect(this, &QTabWidget::tabCloseRequested, [this](int index) {
        WebPageForm *form = reinterpret_cast<WebPageForm *>(widget(index));
        qDebug() << "close tab(" << index << "):" << form->url();
        delete form;
    });
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
