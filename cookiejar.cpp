#include "cookiejar.h"

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtNetwork/QNetworkCookie>

#define SETTING_FILE_NAME       "settings.ini"
#define SETTING_FILE_FORMAT     QSettings::IniFormat

class CookieJar::Private
{
public:
    Private(CookieJar *parent);

private:
    CookieJar *q;
public:
    QSettings settings;
};

CookieJar::Private::Private(CookieJar *parent)
    : q(parent)
    , settings(SETTING_FILE_NAME, SETTING_FILE_FORMAT)
{
    settings.beginGroup(QStringLiteral("cookies"));
}

CookieJar::CookieJar(QObject *parent)
    : QNetworkCookieJar(parent)
    , d(new Private(this))
{
    connect(this, &CookieJar::destroyed, [this](){ delete d; });
}

QList<QNetworkCookie> CookieJar::cookiesForUrl(const QUrl& url) const
{
    QList<QNetworkCookie> ret;
    foreach (const QString &group, d->settings.childGroups()) {
        d->settings.beginGroup(group);
        QNetworkCookie cookie;
        cookie.setDomain(d->settings.value(QStringLiteral("Domain")).toString());
        cookie.setExpirationDate(d->settings.value(QStringLiteral("ExpirationDate")).toDateTime());
        cookie.setHttpOnly(d->settings.value(QStringLiteral("HttpOnly")).toBool());
        cookie.setName(d->settings.value(QStringLiteral("Name")).toByteArray());
        cookie.setPath(d->settings.value(QStringLiteral("Path")).toString());
        cookie.setSecure(d->settings.value(QStringLiteral("Secure")).toBool());
        cookie.setValue(d->settings.value(QStringLiteral("Value")).toByteArray());
        ret.append(cookie);
        d->settings.endGroup();
    }

    return ret;
}

bool CookieJar::deleteCookie(const QNetworkCookie &cookie)
{
    qDebug() << Q_FUNC_INFO << __LINE__ << cookie;
    bool ret = true;
    if (d->settings.childGroups().contains(cookie.name())) {
        d->settings.beginGroup(cookie.name());
        d->settings.remove(QString()); // see QSettings::clear() documentation
        d->settings.endGroup();
    }
    return ret;
}

bool CookieJar::insertCookie(const QNetworkCookie &cookie)
{
    bool ret = true;

    d->settings.beginGroup(cookie.name());
    d->settings.setValue(QStringLiteral("Domain"), cookie.domain());
    d->settings.setValue(QStringLiteral("ExpirationDate"), cookie.expirationDate());
    d->settings.setValue(QStringLiteral("HttpOnly"), cookie.isHttpOnly());
    d->settings.setValue(QStringLiteral("Name"), cookie.name());
    d->settings.setValue(QStringLiteral("Path"), cookie.path());
    d->settings.setValue(QStringLiteral("Secure"), cookie.isSecure());
    d->settings.setValue(QStringLiteral("Value"), cookie.value());
    d->settings.endGroup();

    return ret;
}

bool CookieJar::setCookiesFromUrl(const QList<QNetworkCookie>& cookieList, const QUrl &url)
{
    bool ret = true;

    foreach (const QNetworkCookie &cookie, cookieList) {
        insertCookie(cookie);
    }

    return ret;
}

bool CookieJar::updateCookie(const QNetworkCookie &cookie)
{
    bool ret = true;

    d->settings.beginGroup(cookie.name());
    d->settings.setValue(QStringLiteral("Domain"), cookie.domain());
    d->settings.setValue(QStringLiteral("ExpirationDate"), cookie.expirationDate());
    d->settings.setValue(QStringLiteral("HttpOnly"), cookie.isHttpOnly());
    d->settings.setValue(QStringLiteral("Name"), cookie.name());
    d->settings.setValue(QStringLiteral("Path"), cookie.path());
    d->settings.setValue(QStringLiteral("Secure"), cookie.isSecure());
    d->settings.setValue(QStringLiteral("Value"), cookie.value());
    d->settings.endGroup();

    return ret;
}
