#ifndef RECODINGTHREAD_H
#define RECODINGTHREAD_H

#include <QThread>

#include "webview.h"

class RecodingThread : public QThread
{
    Q_OBJECT
public:
    explicit RecodingThread(QObject *parent = 0);


    WebView *webView() const;
    void setWebView(WebView *webView);

signals:

public slots:

private:
    WebView *m_webView;

    void capture(unsigned long count);

protected:
    void run();
};

#endif // RECODINGTHREAD_H
