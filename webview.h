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
#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QtWebKitWidgets/QWebView>

class WebView : public QWebView
{
    Q_OBJECT
    Q_ENUMS(ViewMode)
    Q_PROPERTY(ViewMode viewMode READ viewMode WRITE setViewMode NOTIFY viewModeChanged)
public:
    enum ViewMode {
        NormalMode,
        FullScreenMode
    };

    explicit WebView(QWidget* parent = 0);

    bool gameExists() const;
    QRect getGameRect() const;

    ViewMode viewMode() const;

public slots:
    void setViewMode(ViewMode viewMode);

signals:
    void viewModeChanged(ViewMode viewMode);

private:
    class Private;
    Private *d;
};

#endif // WEBVIEW_H
