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
#include "mainwindow.h"
#include <QApplication>
#include <QtCore/QTranslator>
#include <QtCore/QLocale>
#include <QQmlEngine>

#include "operatingsystem.h"
#include "clipcursor.h"
#include "qmlfile.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#if defined(Q_OS_MAC)
    //Mac OS X 用のパスを作成する
    QString dir = QString("%1/i18n")
            .arg(QCoreApplication::applicationDirPath());
#else
    //Windows とLinux 用のパスを作成する
    QString dir("i18n");
#endif
    QTranslator translator;
    //言語ファイルの読み込み
    translator.load(QString("qt_%1" ).arg(QLocale::system().name()), dir);
    //言語データを登録する
    app.installTranslator(&translator);
    //Qt標準機能の日本語化(QWebPageとか)
    QTranslator translator2;
    translator2.load(QString("qt_ja" ), dir);
    app.installTranslator(&translator2);

    qmlRegisterType<OperatingSystem>("jp.relog.plugin.operatingsystem"
                                     , 1, 0, "OperatingSystem");
    qmlRegisterType<ClipCursor>("jp.relog.plugin.clipcursor"
                                , 1, 0, "ClipCursor");
    qmlRegisterType<QMLFile>("jp.relog.plugin.qmlfile", 1, 0, "QMLFile");

    MainWindow w;
    w.show();
    
    return app.exec();
}
