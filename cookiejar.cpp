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
#include "cookiejar.h"
#include "kanmusumemory_global.h"
#include <QtCore/QSettings>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtCore/QMutableListIterator>
#include <QtNetwork/QNetworkCookie>

static const unsigned int COOKIEJAR_MAGIC = 80;
static const int INTERVAL = 1000 * 3;  // 3sec.

QDataStream &operator<<(QDataStream &st, const QList<QNetworkCookie> &src)
{
    st << COOKIEJAR_MAGIC;
    st << quint32(src.size());
    for (int i = 0; i < src.size(); ++i) {
        st << src.at(i).toRawForm();
    }
    return st;
}

QDataStream &operator>>(QDataStream &st, QList<QNetworkCookie> &dest)
{
    QList<QNetworkCookie> tmp;
    quint32 magic;
    st >> magic;
    if (magic != COOKIEJAR_MAGIC) {
        return st;
    }

    quint32 count;
    st >> count;

    tmp.reserve(count);
    for (quint32 i = 0; i < count; ++i) {
        QByteArray value;
        st >> value;

        QList<QNetworkCookie> newCookies = QNetworkCookie::parseCookies(value);
        for (int j = 0; j < newCookies.count(); ++j) {
            tmp.append(newCookies.at(j));
        }
        if (st.atEnd()) {
            break;
        }
    }
    dest.swap(tmp);
    return st;
}


CookieJar::CookieJar(QObject *parent)
    : QNetworkCookieJar(parent), saveDelayTimer_(new QTimer(parent))
{
    qRegisterMetaTypeStreamOperators<QList<QNetworkCookie> >("QList<QNetworkCookie>");
    saveDelayTimer_->setInterval(INTERVAL);
    saveDelayTimer_->setSingleShot(true);
    connect(saveDelayTimer_, SIGNAL(timeout()), SLOT(autoSave()));

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, KANMEMO_PROJECT, KANMEMO_NAME);
    dir_ = QFileInfo(settings.fileName()).dir().path();
    settings.remove(QLatin1String("cookies")); // remove old version.
    load();
}

CookieJar::~CookieJar()
{
    saveDelayTimer_->stop();
    save();
}

bool CookieJar::deleteCookie(const QNetworkCookie & cookie)
{
    bool changed = QNetworkCookieJar::deleteCookie(cookie); 
    if (changed) {
        markChanged();
    }
    return changed;
}

bool CookieJar::insertCookie(const QNetworkCookie & cookie)
{
    bool changed = QNetworkCookieJar::insertCookie(cookie); 
    if (changed) {
        markChanged();
    }
    return changed;
}

bool CookieJar::setCookiesFromUrl(const QList<QNetworkCookie> & cookieList, const QUrl & url)
{
    bool changed = QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
    if (changed) {
        markChanged();
    }
    return changed;
}

bool CookieJar::updateCookie(const QNetworkCookie & cookie)
{
    bool changed = QNetworkCookieJar::updateCookie(cookie);
    if (changed) {
        markChanged();
    }
    return changed;
}

void CookieJar::save()
{
    removeExpiredCookies();
    QList<QNetworkCookie> cookies = QNetworkCookieJar::allCookies();
    QMutableListIterator<QNetworkCookie> i(cookies);
    while (i.hasNext()) {
        if ( i.next().isSessionCookie() ) {
            i.remove();
        }
    }
    QSettings cookieSettings(cookiesFile(), QSettings::IniFormat);
    cookieSettings.setValue(QLatin1String("cookies"), QVariant::fromValue<QList<QNetworkCookie> >(cookies));
    modified_ = false;
}

void CookieJar::load()
{
    QSettings cookieSettings(cookiesFile(), QSettings::IniFormat);
    QList<QNetworkCookie> cookies = qvariant_cast<QList<QNetworkCookie> >(cookieSettings.value(QLatin1String("cookies")));
    if (cookies.isEmpty()) {
        return;
    }
    setAllCookies(cookies);
}

void CookieJar::deleteAll()
{
    QList<QNetworkCookie> cookies;
    QSettings cookieSettings(cookiesFile(), QSettings::IniFormat);
    cookieSettings.setValue(QLatin1String("cookies"), QVariant::fromValue<QList<QNetworkCookie> >(cookies));
    setAllCookies(cookies);
}

void CookieJar::autoSave()
{
    if (!modified_) { 
        return; 
    }
    save();
}

void CookieJar::removeExpiredCookies()
{
    QDateTime now = QDateTime::currentDateTime();
    QList<QNetworkCookie> cookies = QNetworkCookieJar::allCookies();
    int count = cookies.count();
    QMutableListIterator<QNetworkCookie> i(cookies);
    while (i.hasNext()) {
        if (i.next().isSessionCookie()) {
            continue;
        }
        if (i.value().expirationDate() < now) {
            i.remove();
        }
    }
    if (cookies.count() != count) {
        setAllCookies(cookies);
    }
}

void CookieJar::markChanged()
{
    if (saveDelayTimer_->isActive()) {
        saveDelayTimer_->stop();
    }
    modified_ = true;
    saveDelayTimer_->start();
}

QString CookieJar::cookiesFile() const 
{
    return QDir(dir_).filePath(QLatin1String("cookies.ini"));
}
