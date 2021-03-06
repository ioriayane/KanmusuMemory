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
#include "imagelabel.h"

#include <QFile>
#include <QDebug>
#include <QPainter>

ImageLabel::ImageLabel(QWidget *parent) :
    QLabel(parent)
{
}

void ImageLabel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    update();
}

void ImageLabel::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    update();
}

QRect ImageLabel::imageRect() const
{
    return m_imageRect;
}
void ImageLabel::setImageRect(const QRect &imageRect)
{
    if(m_imageRect == imageRect)    return;
    m_imageRect = imageRect;
    emit imageRectChanged(m_imageRect);
}


void ImageLabel::update()
{
    if(!QFile::exists(fileName()))  return;

    int top;
    int left;
    QImage img1(fileName());
    QImage img2(img1.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QImage back(width(), height(), QImage::Format_ARGB32);
    QPainter painter(&back);

    back.fill(QColor(0, 0, 0, 0));

    if(img2.width() == width()){
        //上位置を調節
        top = height() / 2 - img2.height() / 2;
        left = 0;
    }else{
        //横位置を調節
        top = 0;
        left = width() / 2 - img2.width() / 2;
    }
    painter.drawImage(left, top, img2);

    setPixmap(QPixmap::fromImage(back));
    setImageRect(QRect(left, top, img2.width(), img2.height()));

}

QString ImageLabel::fileName() const
{
    return m_fileName;
}

void ImageLabel::setFileName(const QString &fileName)
{
    if(m_fileName == fileName)  return;
    m_fileName = fileName;
    update();
}

