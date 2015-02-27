/*
 * Copyright 2013-2015 KanMemo Project.
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
#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabel(QWidget *parent = 0);

    QString fileName() const;
    void setFileName(const QString &fileName);

    QRect imageRect() const;
    void setImageRect(const QRect &imageRect);

signals:
    void imageRectChanged(const QRect &imageRect);

public slots:

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent *event);

private:
    QString m_fileName;
    QRect m_imageRect;

    void update();
};

#endif // IMAGELABEL_H
