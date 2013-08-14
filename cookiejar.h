#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <QtNetwork/QNetworkCookieJar>

class CookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    explicit CookieJar(QObject *parent = 0);
    
    virtual QList<QNetworkCookie> cookiesForUrl(const QUrl& url) const;
    virtual bool deleteCookie(const QNetworkCookie &cookie);
    virtual bool insertCookie(const QNetworkCookie &cookie);
    virtual bool setCookiesFromUrl(const QList<QNetworkCookie>& cookieList, const QUrl &url);
    virtual bool updateCookie(const QNetworkCookie &cookie);
private:
    class Private;
    Private *d;
};

#endif // COOKIEJAR_H
