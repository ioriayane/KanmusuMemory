/*
 * Copyright 2013 IoriAYANE
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

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

private:
    Q_DISABLE_COPY(MainWindow)
    class Private;
    Private *d;

private slots:
    void captureGame();         //保存する
    void calibration();         //flashの位置を調べる

public slots:
    //スクロール状況
    void scrollRequested(int dx, int dy, const QRect &rectToScroll);
};

#endif // MAINWINDOW_H
