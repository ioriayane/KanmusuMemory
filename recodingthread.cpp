#include "recodingthread.h"

#include <QtCore/QDebug>

RecodingThread::RecodingThread(QObject *parent) :
    QThread(parent)
  , m_webView(NULL)
  , m_timer(NULL)
  , m_recodingCounter(0)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [this](){
        //キャプチャ
        unsigned long counter = recodingCounter();
        qDebug() << "trigger timer " << counter;
        capture(counter);
        setRecodingCounter(counter + 1);

        //保存スレッド開始
        start();
    });
}

void RecodingThread::startTimer()
{
    m_recodingCounter = 0;
    m_SaveDataList.clear();
    m_timer->start(100);
}

void RecodingThread::stopTimer()
{
    m_timer->stop();

    foreach (SaveData data, m_SaveDataList) {
        qDebug() << data.image.isNull() << "," << data.path;
    }
}
WebView *RecodingThread::webView() const
{
    return m_webView;
}

void RecodingThread::setWebView(WebView *webview)
{
    m_webView = webview;
}
unsigned long RecodingThread::recodingCounter() const
{
    return m_recodingCounter;
}

void RecodingThread::setRecodingCounter(unsigned long recodingCounter)
{
    m_recodingCounter = recodingCounter;
}


void RecodingThread::capture(unsigned long count)
{
    QImage img = webView()->capture();
    if(img.isNull()){
        qDebug() << "don't capture";
        return;
    }
    SaveData data(img, QString("c:\\temp\\kanmemo_%1.jpg").arg(count, 6, 10, QChar('0')));

    m_mutex.lock();
    m_SaveDataList.append(data);
    m_mutex.unlock();
}




void RecodingThread::run()
{
//    qDebug() << "start recoding thread";

    bool empty;

    m_mutex.lock();
    empty = m_SaveDataList.isEmpty();
    m_mutex.unlock();
    while(!empty){
        SaveData data = m_SaveDataList.first();

        if(data.image.isNull()){
            qDebug() << "list data is null";
        }else if(data.image.save(data.path, "jpg")){
            //ok
        }else{
            //ng
            qDebug() << "failed save";
        }

        m_mutex.lock();
        m_SaveDataList.removeFirst();
        empty = m_SaveDataList.isEmpty();
        m_mutex.unlock();
    }

//    qDebug() << "stop recoding thread";
}
