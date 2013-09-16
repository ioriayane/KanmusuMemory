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
#include "imageeditdialog.h"
#include "ui_imageeditdialog.h"

#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>

class ImageEditDialog::Private
{
public:
    Private(const QString &sourceImagePath
            , const QString &tempSourceImagePath
            , const QString &editImagePath
            , ImageEditDialog *parent);

private:
    ImageEditDialog *q;
    Ui::ImageEditDialog ui;
    QQuickView *view;
public:
    QString sourceImagePath;        //元の画像を残すときのパス
    QString tempSourceImagePath;    //元の画像をテンポラリに保存したパス
    QString editImagePath;          //編集後の画像

    NextOperationType nextOperation;
};

ImageEditDialog::Private::Private(const QString &sourceImagePath
                               , const QString &tempSourceImagePath
                               , const QString &editImagePath
                               , ImageEditDialog *parent)
    : q(parent)
    , view(new QQuickView)
{
    ui.setupUi(q);
    connect(view->engine(), &QQmlEngine::quit, [this]() {
        this->sourceImagePath = view->rootContext()->contextProperty("sourceImagePath").toString();
        this->tempSourceImagePath = view->rootContext()->contextProperty("tempSourceImagePath").toString();
        this->editImagePath = view->rootContext()->contextProperty("editImagePath").toString();
        switch(view->rootObject()->property("nextOperation").toInt()){
        case 0:
            nextOperation = SaveAndTweet;
            break;
        case 1:
            nextOperation = Save;
            break;
        default:
            nextOperation = None;
            break;
        }
        q->accept();
    });

    //C++のデータをQML側へ公開
    view->rootContext()->setContextProperty("sourceImagePath", sourceImagePath);
    view->rootContext()->setContextProperty("tempSourceImagePath", tempSourceImagePath);
    view->rootContext()->setContextProperty("editImagePath", editImagePath);

    //QML設定して表示
    view->setSource(QUrl("qrc:///qml/KanmusuMemory/imageEditDialog.qml"));
    ui.layout->addWidget(QWidget::createWindowContainer(view, q));
}


ImageEditDialog::ImageEditDialog(const QString &sourceImagePath
                                 , const QString &tempSourceImagePath
                                 , const QString &editImagePath
                                 , QWidget *parent) :
    QDialog(parent)
  , d(new Private(sourceImagePath, tempSourceImagePath, editImagePath, this))
{
    connect(this, &QObject::destroyed, [this]() { delete d; });
}

const QString &ImageEditDialog::sourceImagePath()
{
    return d->sourceImagePath;
}

const QString &ImageEditDialog::tempSourceImagePath()
{
    return d->tempSourceImagePath;
}

const QString &ImageEditDialog::editImagePath()
{
    return d->editImagePath;
}

const ImageEditDialog::NextOperationType &ImageEditDialog::nextOperation()
{
    return d->nextOperation;
}



