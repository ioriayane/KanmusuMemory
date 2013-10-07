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
#ifndef TABWIDGET_H
#define TABWIDGET_H

#include "webpageform.h"

#include <QTabWidget>

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget* parent = 0);

    bool saveOpenPage() const;
    void setSaveOpenPage(bool saveOpenPage);
    bool openAndNewTab() const;
    void setOpenAndNewTab(bool openAndNewTab);

    void openUrl(const QUrl &url);
    void newTab(const QUrl &url);
    void closeTab();
    void closeTab(WebPageForm *form);
    void closeTabAll();
    void reloadTab();

    void load();
    void save();


private:
    class Private;
    Private *d;

    bool m_saveOpenPage;
    bool m_openAndNewTab;
};

#endif // TABWIDGET_H
