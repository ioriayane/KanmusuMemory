#ifndef RECODINGTHREAD_H
#define RECODINGTHREAD_H

#include <QThread>
#include <QTimer>
#include <QMutex>

#include "webview.h"


class SaveData
{
public:
    explicit SaveData(QImage img, QString path)
        : image(img), path(path)
    {
    }

    QImage image;
    QString path;
};

class RecodingThread : public QThread
{
    Q_OBJECT
public:
    explicit RecodingThread(QObject *parent = 0);


    void startTimer();
    void stopTimer();

    WebView *webView() const;
    void setWebView(WebView *webView);

    unsigned long recodingCounter() const;
    void setRecodingCounter(unsigned long recodingCounter);


signals:

public slots:

private:
    WebView *m_webView;
    QTimer *m_timer;
    unsigned long m_recodingCounter;

    QMutex m_mutex;
    QList<SaveData> m_SaveDataList;

    void capture(unsigned long count);

protected:
    void run();
};

#endif // RECODINGTHREAD_H
