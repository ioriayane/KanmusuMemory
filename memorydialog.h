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
#ifndef MEMORYDIALOG_H
#define MEMORYDIALOG_H

#include <QDialog>
#include "qtquick2applicationviewer.h"
#include "memorydata.h"

namespace Ui {
class MemoryDialog;
}

class MemoryDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MemoryDialog(QWidget *parent = 0);
    ~MemoryDialog();
    
    void setMemoryPath(const QString &path);
    const QString &imagePath();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent *event);

public slots:
    void closeQml();

private:
    Ui::MemoryDialog *ui;

    QtQuick2ApplicationViewer *m_viewer;
    MemoryData m_data;
    QString m_memoryPath;   //保存パス
    QString m_imagePath;    //つぶやく対象の画像
};

#endif // MEMORYDIALOG_H
