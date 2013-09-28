#include "webpageform.h"
#include "ui_webpageform.h"

#include <QWebView>
#include <QTabWidget>

#include <QDebug>

class webPage : public QWebPage {
public:
    explicit webPage(QObject * parent = 0):QWebPage(parent){  }

    QString userAgentForUrl(const QUrl &url ) const {
        return QWebPage::userAgentForUrl(url) + QString("; Android");
//        return QString("Mozilla/5.0 (Windows NT 6.2; Mobile; WOW64) AppleWebKit/537.21 (KHTML, like Gecko) KanmusuMemory Safari/537.21");
//        return QString("Mozilla/5.0 (Android; Mobile; WOW64) AppleWebKit/537.21 (KHTML, like Gecko) KanmusuMemory Safari/537.21");
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
        //        if (ok) {
        //            ui.statusBar->showMessage(MainWindow::tr("complete"), STATUS_BAR_MSG_TIME);
        //        } else {
        //            ui.statusBar->showMessage(MainWindow::tr("error"), STATUS_BAR_MSG_TIME);
        //        }
        ui.progressBar->hide();

        //タイトル更新
//        setParentTitle(ui.webView->title());
    });
    //WebViewの読込み状態
    connect(ui.webView, &QWebView::loadProgress, ui.progressBar, &QProgressBar::setValue);

    //URLが外から変更された
    connect(q, &WebPageForm::urlChanged, [this]() {
        webPage *page = new webPage;
        ui.webView->setPage((QWebPage *)page);
        ui.webView->setUrl(q->url());
        ui.urlEdit->setText(q->url().toString());
    });

    //URLの編集完了
    connect(ui.urlEdit, &QLineEdit::editingFinished, [this]() {
        q->setUrl(ui.urlEdit->text());
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

QUrl WebPageForm::url() const
{
    return m_url;
}
void WebPageForm::setUrl(const QUrl &url)
{
    if(m_url == url)
        return;

    m_url = url;
    emit urlChanged();
}

QString WebPageForm::title() const
{
    return d->ui.webView->title();
}

