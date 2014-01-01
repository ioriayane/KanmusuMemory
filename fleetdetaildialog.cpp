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
#include "fleetdetaildialog.h"
#include "ui_fleetdetaildialog.h"
#include "kanmusumemory_global.h"

#include <QtCore/QDebug>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>
#include <QtCore/qmath.h>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>

class FleetDetailDialog::Private
{
public:
    Private(FleetDetailDialog *parent
            , WebView *webview
            , QRect capture_rect
            , qreal view_ratio
            , int columns);
    ~Private();

private:
    FleetDetailDialog *q;
    Ui::FleetDetailDialog ui;
    QQuickView *view;
    int columns;                //プレビューの列数
    qreal viewRatio;            //プレビューの表示倍率
    QRect captureRect;          //取り込む範囲
public:
    NextOperationType nextOperation;
    CaptureInterface capture;

    QString combinImagePath;    //結合した画像のパス
};


FleetDetailDialog::Private::Private(FleetDetailDialog *parent
                                    , WebView *webview
                                    , QRect capture_rect
                                    , qreal view_ratio
                                    , int columns)
    : q(parent)
    , view(new QQuickView)
    , columns(columns)
    , viewRatio(view_ratio)
    , captureRect(capture_rect)
{
    ui.setupUi(q);
    capture.webview = webview;

    connect(view->engine(), &QQmlEngine::quit, [this]() {

        QVariantList list = view->rootObject()->property("imageList").toList();
        switch(view->rootObject()->property("nextOperation").toInt()){
        case 0:
        {
            nextOperation = Combine;
            QStringList slist;
            foreach (QVariant item, list) {
                slist.append(item.toString());
            }

            //結合は上位でする
            emit q->finishedCaptureImages(slist, captureRect.width(), captureRect.height(), this->columns);
            break;
        }
        default:
            nextOperation = None;
            break;
        }
        //終了
        q->accept();
    });


    //クラスを登録して関数を使えるようにする
    view->rootContext()->setContextProperty("CaptureInterface", &capture);
    //QML設定して表示
    view->setSource(QUrl("qrc:///qml/KanmusuMemory/fleetDetailDialog.qml"));
    ui.layout->addWidget(QWidget::createWindowContainer(view, q));
    //コンテンツのサイズ
    view->rootObject()->setProperty("columns", this->columns);
    view->rootObject()->setProperty("viewRatio", viewRatio);
    view->rootObject()->setProperty("contentX", captureRect.x());
    view->rootObject()->setProperty("contentY", captureRect.y());
    view->rootObject()->setProperty("contentWidth", captureRect.width());
    view->rootObject()->setProperty("contentHeight", captureRect.height());
    //ウインドウの最小サイズを調節
    QSize contentSize = view->rootObject()->childrenRect().toRect().size();
    contentSize.setWidth(view->rootObject()->property("width").toInt());
    q->setMinimumSize(contentSize);
//    q->setMaximumSize(contentSize);

    //QMLのサイズが追従するようにする
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    //    view->setResizeMode(QQuickView::SizeViewToRootObject);
}

FleetDetailDialog::Private::~Private()
{
    QString path = QString("%1/%2/%3")
            .arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
            .arg(KANMEMO_PROJECT)
            .arg("combine");
    QDir dir(path);
    QFileInfoList list = dir.entryInfoList();
    //ファイルを消す
    for(int i=0; i<list.length(); i++){
        QFile::remove(list[i].filePath());
    }
}


//キャプチャする
QString CaptureInterface::captureGame(qreal x, qreal y, qreal w, qreal h)
{
    static long number = 0;

    if(webview == NULL)
        return QString();

    //キャプチャ
    QImage img = webview->capture();
    if (img.isNull())
        return QString();
    //クリップ
    QImage clipImg = img.copy(QRect(x, y, w, h));


    QString path = QString("%1/%2/%3")
            .arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
            .arg(KANMEMO_PROJECT)
            .arg("combine");
    //なければつくる
    QDir dir(path);
    if(!dir.exists())
        dir.mkdir(path);
    //ファイル名
    QString filename = QString("%1/capture_temp_%3.png").arg(path).arg(number++);
    //保存
    clipImg.save(filename);

    return filename;
}


FleetDetailDialog::FleetDetailDialog(WebView *webview
                                     , QRect capture_rect
                                     , qreal view_ratio
                                     , int columns
                                     , QWidget *parent) :
    QDialog(parent)
  , d(new Private(this, webview, capture_rect, view_ratio, columns))
{
    connect(this, &QObject::destroyed, [this]() { delete d; });
}

FleetDetailDialog::~FleetDetailDialog()
{
}



