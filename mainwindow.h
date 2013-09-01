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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QNetworkReply>
#include <QSslError>
#include "webpageoperation.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

    bool isFullScreen() const;

private:
    Q_DISABLE_COPY(MainWindow)
    class Private;
    Private *d;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors);
    virtual void resizeEvent(QResizeEvent * event);
};

#endif // MAINWINDOW_H
