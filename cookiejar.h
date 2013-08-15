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
