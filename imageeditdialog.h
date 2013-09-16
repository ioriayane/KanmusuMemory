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
#ifndef IMAGEEDITDIALOG_H
#define IMAGEEDITDIALOG_H

#include <QDialog>

class ImageEditDialog : public QDialog
{
    Q_OBJECT
    
public:
    enum NextOperationType {
        None
        , SaveAndTweet
        , Save
    };

    explicit ImageEditDialog(const QString &sourceImagePath
                             , const QString &tempSourceImagePath
                             , const QString &editImagePath
                             , QWidget *parent = 0);
    
    const QString &sourceImagePath();
    const QString &tempSourceImagePath();
    const QString &editImagePath();
    const NextOperationType &nextOperation();

private:
    class Private;
    Private *d;
};

#endif // IMAGEEDITDIALOG_H
