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
#ifndef WEBPAGEOPERATION_H
#define WEBPAGEOPERATION_H

#include <QHash>
#include <QRect>

class QWidget;
class QWebView;

class WebPageOperation
{
public:
    enum ViewMode {
        NormalMode,
        FullScreenMode
    };

    explicit WebPageOperation(QWebView *webView);
    
    bool gameExists() const;
    QRect getGameRect() const;
    void setViewMode(ViewMode viewMode);

private:
    void showNormal();
    void showFullScreen();

private:
    QWebView *webView;

    QHash<QString, QString> m_body;
    QHash<QString, QString> m_gameFrame;
    QHash<QString, QString> m_flashWrap;
    QHash<QString, QString> m_embed;
    QHash<QString, QString> m_sectionWrap;
};

#endif // WEBPAGEOPERATION_H
