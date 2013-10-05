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
#include "ui_webpageform.h"

#include <QWebView>
#include <QTabWidget>
#include <QNetworkRequest>

#include <QDebug>

class webPage : public QWebPage {
public:
    explicit webPage(QObject * parent = 0)
        :QWebPage(parent)
        , isMobileMode(true)
    {  }

    bool isMobileMode;

    //USER AGENTを調節
    QString userAgentForUrl(const QUrl &url ) const {
        QString ret = QWebPage::userAgentForUrl(url);
        if(isMobileMode)
            ret.append(QStringLiteral("; Android"));

        return ret;
//        return QString("Mozilla/5.0 (Windows NT 6.2; Mobile; WOW64) AppleWebKit/537.21 (KHTML, like Gecko) KanmusuMemory Safari/537.21");
//        return QString("Mozilla/5.0 (Android; Mobile; WOW64) AppleWebKit/537.21 (KHTML, like Gecko) KanmusuMemory Safari/537.21");
    }
    //リンクに関連する操作
    bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type) {
        qDebug() << "accept navi page:" << request.url() << "/" << type << "/frame=" << frame << "/this=" << this;
        return QWebPage::acceptNavigationRequest(frame, request, type);
    }
    //ウインドウ作成
    QWebPage* createWindow(WebWindowType type) {
        qDebug() << "createWindow: " << type;
        return new webPage();
    }
    //リンククリック
    void linkClicked(const QUrl & url) {
        qDebug() << "linkClicked:" << url;
    }
};

class WebPageForm::Private
{
public:
    Private(WebPageForm *parent);

    void setTab(QTabWidget *tab);
private:
    WebPageForm *q;
    QTabWidget *tab;

    void setParentTitle(QString &title);
public:
    Ui::WebPageForm ui;
};

WebPageForm::Private::Private(WebPageForm *parent)
    : q(parent)
    , tab(NULL)
{
    ui.setupUi(q);

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
        setParentTitle(QString("%1...").arg(ui.webView->title().left(5)));
    });
    //WebViewの読込み状態
    connect(ui.webView, &QWebView::loadProgress, ui.progressBar, &QProgressBar::setValue);

    //URLが変更された
    connect(ui.webView, &QWebView::urlChanged, [this]() {
        qDebug() << "change url:" << ui.webView->url();
        ui.urlEdit->setText(ui.webView->url().toString());
    });

    //URLの編集完了
    connect(ui.urlEdit, &QLineEdit::editingFinished, [this]() {
//        q->setUrl(ui.urlEdit->text());
        webPage *page = new webPage;
        ui.webView->setPage((QWebPage *)page);
        ui.webView->setUrl(ui.urlEdit->text());
//        ui.urlEdit->setText(q->url().toString());

    });

    //閉じる要求


    //モバイルとPC版の切り換え
    connect(ui.changeMobileModeButton, &QPushButton::clicked, [this]() {
        webPage *page = reinterpret_cast<webPage *>(ui.webView->page());
        page->isMobileMode = !page->isMobileMode;
        qDebug() << "USER_AGENT:" << page->userAgentForUrl(ui.webView->url());
        ui.webView->reload();
    });

}

void WebPageForm::Private::setTab(QTabWidget *tab)
{
    this->tab = tab;
}

void WebPageForm::Private::setParentTitle(QString &title)
{
    if(tab == NULL)
        return;

    int index = tab->indexOf(q);
    if(index > -1){
        tab->setTabText(index, title);
    }
}




WebPageForm::WebPageForm(QWidget *parent) :
    QWidget(parent),
    d(new Private(this))
{
    d->setTab((QTabWidget *)parent);
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
//    d->ui.webView->setUrl(url);
    //外から設定するときはURLのエディットを修正したことにする
    d->ui.urlEdit->setText(url.toString());
    d->ui.urlEdit->editingFinished();
}
//表示中のWebページのタイトル
QString WebPageForm::title() const
{
    return d->ui.webView->title();
}
//リロード
void WebPageForm::reload()
{
    d->ui.webView->reload();
}

