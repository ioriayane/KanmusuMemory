#include "recodingthread.h"

#include <QtCore/QDebug>

RecodingThread::RecodingThread(QObject *parent) :
    QThread(parent)
  , m_webView(NULL)
{
}
WebView *RecodingThread::webView() const
{
    return m_webView;
}

void RecodingThread::setWebView(WebView *webview)
{
    m_webView = webview;
}

void RecodingThread::capture(unsigned long count)
{
    QImage img = webView()->capture();
    if(img.isNull()){
        qDebug() << "don't capture";
        return;
    }
    QString path = QString("d:\\temp\\kanmemo_%1.png").arg(count);
    if(img.save(path, "png")){
        //save ok
    }else{
        qDebug() << "save fail";
    }
}




void RecodingThread::run()
{
    qDebug() << "start recoding thread";

    unsigned long i;

    if(webView() == NULL){
        qDebug() << "not found webview";
    }

    i = 0;
    while(i < 10){
        capture(i);
        i++;
        msleep(33);
    }


    qDebug() << "stop recoding thread";
}
