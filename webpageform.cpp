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

class webPage : public QWebPage {
public:
    explicit webPage(QObject * parent)
        :QWebPage(parent)
    {
        webpageform = reinterpret_cast<WebPageForm *>(parent);
    }

public:

    //USER AGENTを調節
    QString userAgentForUrl(const QUrl &url ) const {
        QString ret = QWebPage::userAgentForUrl(url);
        if(webpageform->isMobileMode()){
#if defined(Q_OS_WIN)
//            ret = QStringLiteral("Mozilla/5.0 (Windows Phone OS 7.5;) AppleWebKit/537.21 (KHTML, like Gecko) KanmusuMemory Safari/537.21");
            ret = QStringLiteral("Mozilla/5.0 (Linux; U; Android 4.1.1; ja-jp;) AppleWebKit/537.21 (KHTML, like Gecko) KanmusuMemory Safari/537.21");
#elif defined(Q_OS_LINUX)
            ret = QStringLiteral("Mozilla/5.0 (Linux; U; Android 4.1.1; ja-jp;) AppleWebKit/537.21 (KHTML, like Gecko) KanmusuMemory Safari/537.21");
#elif defined(Q_OS_MAC)
            ret = QStringLiteral("Mozilla/5.0 (iPhone; CPU iPhone OS 6_0 like Mac OS X) AppleWebKit/537.21 (KHTML, like Gecko) KanmusuMemory Safari/537.21");
#endif
        }
        return ret;
        //        return QString("Mozilla/5.0 (Windows NT 6.2; Mobile; WOW64) AppleWebKit/537.21 (KHTML, like Gecko) KanmusuMemory Safari/537.21");
        //        return QString("Mozilla/5.0 (Linux; U; Android 4.1.1; ja-jp; Galaxy Nexus Build/JRO03H) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30");
    }
    //リンクに関連する操作
    //    bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type) {
    //        qDebug() << "accept navi page:" << request.url() << "/" << type << "/frame=" << frame << "/this=" << this;
    //        return QWebPage::acceptNavigationRequest(frame, request, type);
    //    }
    //ウインドウ作成
    QWebPage* createWindow(WebWindowType type) {
        Q_UNUSED(type);
        //        qDebug() << "createWindow: " << type;
        webPage *webpage = new webPage(webpageform);
        webpageform->makeNewWebPage(webpage);
        return webpage;
    }

private:
    WebPageForm *webpageform;
};


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
        webPage *page = new webPage(q);
        ui.webView->setPage(reinterpret_cast<QWebPage *>(page));
        ui.webView->setUrl(ui.urlEdit->text());
        connect(page, &QWebPage::windowCloseRequested, [this]() {
            qDebug() << "windowCloseRequested";
            emit q->removeTabRequested(q);
        });
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
        webPage *page = reinterpret_cast<webPage *>(ui.webView->page());
        q->setMobileMode(ui.changeMobileModeButton->isChecked());
        qDebug() << "USER_AGENT:" << page->userAgentForUrl(ui.webView->url());
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
//        qDebug() << list.value(key) << ", " << key;
        if(url == key){
            //見つかった
            exist = true;
            if(add){
                //追加（ただしすでにある）
//                qDebug() << "exist:" << title;
            }else{
                //削除
//                qDebug() << "remove:" << title;
                list.remove(url);
                update = true;
            }
            break;
        }
    }
    if(!exist){
//        qDebug() << "add:" << title;
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
    m_mobileMode(true)
{
    d->setTab(reinterpret_cast<TabWidget *>(parent));

    connect(this, &QObject::destroyed, [this]() { delete d; });

    //    //親がタブだったら
    //    if(typeid(QTabWidget *) == typeid((QTabWidget *)parent)){
    //        d->setTab((QTabWidget *)parent);
    //        qDebug() << "Tab";
    //    }else{
    //        qDebug() << "not Tab " << typeid(parent).name();
    //    }
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
void WebPageForm::setWebPage(QWebPage *webpage)
{
    d->ui.webView->setPage(webpage);
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


void WebPageForm::makeNewWebPage(QWebPage *webpage)
{
    emit addTabRequested(webpage);
}

void WebPageForm::find()
{
    if(d->isFindVisible()){
        d->setFindVisiblity(false);
    }else{
        d->setFindVisiblity(true);
        d->ui.findEdit->setFocus();
    }
}

