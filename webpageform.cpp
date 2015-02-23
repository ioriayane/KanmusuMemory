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
#include "webpageform.h"
#include "tabwidget.h"
#include "ui_webpageform.h"
#include "kanmusumemory_global.h"

#include <QWebView>
#include <QNetworkRequest>
#include <QSettings>
#include <QDebug>



class WebPageForm::Private
{
public:
    Private(WebPageForm *parent);

    void setTab(TabWidget *tab);
    void setFindVisiblity(bool visible);
    bool isFindVisible();
private:
    WebPageForm *q;
    TabWidget *tab;
    QSettings settings;

    void setParentTitle(const QString &title);
    void updateFavorite(const QString &url, const QString &title, bool add);
    bool isExistFavorite(const QString &url);
public:
    Ui::WebPageForm ui;
};

WebPageForm::Private::Private(WebPageForm *parent)
    : q(parent)
    , tab(NULL)
    , settings(QSettings::IniFormat, QSettings::UserScope, KANMEMO_PROJECT, KANMEMO_NAME_FAV)
{
    ui.setupUi(q);

    //検索を非表示
    setFindVisiblity(false);

    //WebViewの読込み開始
    connect(ui.webView, &QWebView::loadStarted, [this](){
        ui.progressBar->show();
    });
    //WebViewの読込み完了
    connect(ui.webView, &QWebView::loadFinished, [this](bool ok) {
        Q_UNUSED(ok);
        //        if (ok) {
        //            ui.statusBar->showMessage(MainWindow::tr("complete"), STATUS_BAR_MSG_TIME);
        //        } else {
        //            ui.statusBar->showMessage(MainWindow::tr("error"), STATUS_BAR_MSG_TIME);
        //        }
        ui.progressBar->hide();

        //タイトル更新
        setParentTitle(QString("%1...").arg(ui.webView->title().left(10)));
    });
    //WebViewの読込み状態
    connect(ui.webView, &QWebView::loadProgress, ui.progressBar, &QProgressBar::setValue);

    //URLが変更された
    connect(ui.webView, &QWebView::urlChanged, [this]() {
        ui.urlEdit->setText(ui.webView->url().toString());
        ui.favoriteButton->setChecked(isExistFavorite(ui.webView->url().toString()));
    });

    //URLの編集完了
    connect(ui.urlEdit, &QLineEdit::editingFinished, [this]() {
        if(ui.webView->url().toString() == ui.urlEdit->text())
            return;
        ui.webView->setUrl(ui.urlEdit->text());
    });
    //戻る
    connect(ui.goBackButton, &QPushButton::clicked, [this]() {
        ui.webView->back();
    });
    //進む
    connect(ui.goForwardButton, &QPushButton::clicked, [this]() {
        ui.webView->forward();
    });
    //リロード
    connect(ui.reloadButton, &QPushButton::clicked, [this]() {
        ui.webView->reload();
    });
    //お気に入り
    connect(ui.favoriteButton, &QPushButton::clicked, [this]() {
        updateFavorite(ui.webView->url().toString(), ui.webView->title(), ui.favoriteButton->isChecked());
    });

    //モバイルとPC版の切り換え
    connect(ui.changeMobileModeButton, &QPushButton::clicked, [this]() {
//        WebPage *page = reinterpret_cast<WebPage *>(ui.webView->page());
//        qDebug() << "USER_AGENT:" << page->userAgentForUrl(ui.webView->url());
        q->setMobileMode(ui.changeMobileModeButton->isChecked());
        ui.webView->reload();
    });

    //検索
    connect(ui.findEdit, &QLineEdit::textChanged, [this]() {
        ui.webView->findText(QString(), QWebPage::HighlightAllOccurrences);    //ハイライトを消す
        if(ui.webView->findText(ui.findEdit->text())){
            //検索できた
            ui.webView->findText(ui.findEdit->text(), QWebPage::HighlightAllOccurrences);//ハイライトする
        }
    });

}
//タブ（親）を保存
void WebPageForm::Private::setTab(TabWidget *tab)
{
    this->tab = tab;
}
//検索の表示切り換え
void WebPageForm::Private::setFindVisiblity(bool visible)
{
    ui.findButton->setVisible(visible);
    ui.findEdit->setVisible(visible);
}
//検索を表示してるか
bool WebPageForm::Private::isFindVisible()
{
    return ui.findEdit->isVisible();
}
//タブの名称を変更
void WebPageForm::Private::setParentTitle(const QString &title)
{
    if(tab == NULL)
        return;

    int index = tab->indexOf(q);
    if(index > -1){
        tab->setTabText(index, title);
    }
}
//お気に入りを更新
void WebPageForm::Private::updateFavorite(const QString &url, const QString &title, bool add)
{
    bool exist = false;
    bool update = false;

    settings.beginGroup(QStringLiteral(FAV_USER));

    QHash<QString, QVariant> list = settings.value(QStringLiteral(FAV_USER_BOOKMARK)).toHash();
    foreach (const QString &key, list.keys()) {
        if(url == key){
            //見つかった
            exist = true;
            if(add){
                //追加（ただしすでにある）
            }else{
                //削除
                list.remove(url);
                update = true;
            }
            break;
        }
    }
    if(!exist){
        list.insert(url, title);
        update = true;
    }
    if(update)
        settings.setValue(QStringLiteral(FAV_USER_BOOKMARK), list);

    settings.endGroup();
    settings.sync();

    if(update)
        emit q->updateFavorite();
}
//お気に入りに登録済みか確認
bool WebPageForm::Private::isExistFavorite(const QString &url)
{
    bool exist = false;

    settings.beginGroup(QStringLiteral(FAV_USER));

    QHash<QString, QVariant> list = settings.value(QStringLiteral(FAV_USER_BOOKMARK)).toHash();
    foreach (const QString &key, list.keys()) {
        if(url == key){
            //見つかった
            exist = true;
            break;
        }
    }

    settings.endGroup();

    return exist;
}





WebPageForm::WebPageForm(QWidget *parent) :
    QWidget(parent),
    d(new Private(this)),
    m_mobileMode(true),
    m_networkAccessManager(NULL)
{

    d->setTab(reinterpret_cast<TabWidget *>(parent));

    //独自のwebpageにする
    setWebPage(new WebPage(parent));

    //終了
    connect(this, &QObject::destroyed, [this]() { delete d; });

}

WebPageForm::~WebPageForm()
{
}
//URL
QUrl WebPageForm::url() const
{
    return d->ui.webView->url();
}
void WebPageForm::setUrl(const QUrl &url)
{
    //外から設定するときはURLのエディットを修正したことにする
    d->ui.urlEdit->setText(url.toString());
    d->ui.urlEdit->editingFinished();
}
void WebPageForm::setWebPage(WebPage *webpage)
{
    //モバイルモードのフラグ
    webpage->setIsMoibleMode(&m_mobileMode);
    //ページを設定
    d->ui.webView->setPage(webpage);

    //新しいウインドウを作成するときのシグナル
    connect(webpage, &WebPage::createNewWindow, [this](WebPage *webpage){
        emit addTabRequested(webpage);
    });
    //閉じる要求
    connect(d->ui.webView->page(), &QWebPage::windowCloseRequested, [this]() {
        emit removeTabRequested(this);
    });

}
//表示中のWebページのタイトル
QString WebPageForm::title() const
{
    return d->ui.webView->title();
}
//モバイルモードか
bool WebPageForm::isMobileMode() const
{
    return m_mobileMode;
}
void WebPageForm::setMobileMode(bool mobileMode)
{
    m_mobileMode = mobileMode;
    d->ui.changeMobileModeButton->setChecked(mobileMode);
}

//リロード
void WebPageForm::reload()
{
    d->ui.webView->reload();
}
//検索
void WebPageForm::find()
{
    if(d->isFindVisible()){
        d->setFindVisiblity(false);
    }else{
        d->setFindVisiblity(true);
        d->ui.findEdit->setFocus();
    }
}

QNetworkAccessManager *WebPageForm::networkAccessManager() const
{
    return m_networkAccessManager;
}
void WebPageForm::setNetworkAccessManager(QNetworkAccessManager *networkAccessManager)
{
    if(m_networkAccessManager != networkAccessManager && networkAccessManager != NULL){
        m_networkAccessManager = networkAccessManager;
        d->ui.webView->page()->setNetworkAccessManager(m_networkAccessManager);
    }
}




