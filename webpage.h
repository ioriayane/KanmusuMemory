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
#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebPage>

class WebPage : public QWebPage {
    Q_OBJECT

public:
    explicit WebPage(QObject *parent = 0);

public:

    //USER AGENTを調節
    QString userAgentForUrl(const QUrl &url ) const;
    //リンクに関連する操作
    //    bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type) {
    //        qDebug() << "accept navi page:" << request.url() << "/" << type << "/frame=" << frame << "/this=" << this;
    //        return QWebPage::acceptNavigationRequest(frame, request, type);
    //    }
    //ウインドウ作成
    QWebPage* createWindow(WebWindowType type);

    bool isMoibleMode() const;
    void setIsMoibleMode(bool *isMoibleMode);

signals:
    void createNewWindow(WebPage *webpage);

private:
    QObject *parent;
    bool *m_isMoibleMode;
};

#endif // WEBPAGE_H
