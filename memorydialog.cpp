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
#include "memorydialog.h"
#include "ui_memorydialog.h"
#include "thumbnailprovider.h"

#include <QtCore/QDebug>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>

class MemoryDialog::Private
{
public:
    Private(const QString &memoryPath, MemoryDialog *parent);

private:
    MemoryDialog *q;
    Ui::MemoryDialog ui;
    QQuickView *view;
public:
    QString imagePath;    //つぶやく対象の画像
};

MemoryDialog::Private::Private(const QString &memoryPath, MemoryDialog *parent)
    : q(parent)
    , view(new QQuickView)
{
    ui.setupUi(q);
    connect(view->engine(), &QQmlEngine::quit, [this]() {
        imagePath = view->rootObject()->property("imagePath").toString();
        q->accept();
    });
    //プラグインを登録
    view->engine()->addImageProvider(QStringLiteral("thumbnail")
                                      , new ThumbnailProvider);
    //C++のデータをQML側へ公開
    view->rootContext()->setContextProperty("memoryPath", memoryPath);

    //QML設定して表示
    view->setSource(QUrl("qrc:///qml/KanmusuMemory/memoryDialog.qml"));
    ui.layout->addWidget(QWidget::createWindowContainer(view, q));
}

MemoryDialog::MemoryDialog(const QString &memoryPath, QWidget *parent)
    : QDialog(parent)
    , d(new Private(memoryPath, this))
{
    connect(this, &QObject::destroyed, [this]() { delete d; });
}

const QString &MemoryDialog::imagePath()
{
    return d->imagePath;
}
