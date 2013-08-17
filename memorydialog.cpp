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

#include <QQmlContext>
#include <QtQml/QQmlEngine>

MemoryDialog::MemoryDialog(QWidget *parent) :
    QDialog(parent),
    m_viewer(NULL),
    ui(new Ui::MemoryDialog)
{
    ui->setupUi(this);

}

MemoryDialog::~MemoryDialog()
{
    if(m_viewer != NULL)
        delete m_viewer;
    delete ui;
}

void MemoryDialog::setMemoryPath(const QString &path)
{
    m_memoryPath = path;
}

const QString &MemoryDialog::imagePath()
{
    return m_imagePath;
}

void MemoryDialog::resizeEvent(QResizeEvent *event)
{
    if(m_viewer != NULL){
        m_viewer->setGeometry(QRect(0, 0, width(), height()));
    }
}

void MemoryDialog::showEvent(QShowEvent *event)
{
    if(m_viewer == NULL){
        m_viewer = new QtQuick2ApplicationViewer(windowHandle());
        connect(m_viewer->engine(), SIGNAL(quit()), this, SLOT(closeQml()));
        //プラグインを登録
        m_viewer->engine()->addImageProvider(QStringLiteral("thumbnail")
                                          , new ThumbnailProvider);
        //C++のデータをQML側へ公開
        m_data.setMemoryPath(m_memoryPath);
        m_viewer->rootContext()->setContextProperty("memoryData", &m_data);

        //QML設定して表示
        m_viewer->setSource(QUrl("qrc:///qml/KanmusuMemory/memoryDialog.qml"));
        m_viewer->show();
    }
}

void MemoryDialog::closeQml()
{
    m_imagePath = m_data.imagePath();
    accept();
}
