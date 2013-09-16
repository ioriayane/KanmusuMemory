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
#include "clipcursor.h"
#include <QPainter>
#include <QCursor>

ClipCursor::ClipCursor(QQuickPaintedItem *parent) :
    QQuickPaintedItem(parent)
  , m_modify(NoModify)
  , m_freeDrag(false)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
}
//カーソルの位置
const QRect &ClipCursor::cursor() const
{
    return m_cursor;
}
//カーソルの位置
void ClipCursor::setCursor(const QRect &rect)
{
    if(m_cursor == rect)
        return;

    m_cursor = rect;

    //範囲外の調整
    qreal x = m_cursor.x();
    qreal y = m_cursor.y();
    qreal width = m_cursor.width();
    qreal height = m_cursor.height();
    if(availableRange().isValid()){
        if(m_cursor.left() < availableRange().left()){
            m_cursor.setX(availableRange().x());
            m_cursor.setWidth(width);
        }
        if(m_cursor.top() < availableRange().top()){
            m_cursor.setY(availableRange().y());
            m_cursor.setHeight(height);
        }

        if(m_cursor.right() > availableRange().right()){
            m_cursor.setX(availableRange().right() - m_cursor.width() + 1);
            m_cursor.setWidth(width);
        }
        if(m_cursor.bottom() > availableRange().bottom()){
            m_cursor.setY(availableRange().bottom() - m_cursor.height() + 1);
            m_cursor.setHeight(height);
        }
    }else{
        if(m_cursor.left() < boundingRect().left()){
            m_cursor.setX(boundingRect().x());
            m_cursor.setWidth(width);
        }
        if(m_cursor.top() < boundingRect().top()){
            m_cursor.setY(boundingRect().y());
            m_cursor.setHeight(height);
        }

        if(m_cursor.right() > boundingRect().right()){
            m_cursor.setX(boundingRect().right() - m_cursor.width() + 1);
            m_cursor.setWidth(width);
        }
        if(m_cursor.bottom() > boundingRect().bottom()){
            m_cursor.setY(boundingRect().bottom() - m_cursor.height() + 1);
            m_cursor.setHeight(height);
        }
    }

    //相対位置
    x = m_cursor.x();
    y = m_cursor.y();
    width = m_cursor.width();
    height = m_cursor.height();
    if(availableRange().isValid()){
        x -= availableRange().x();
        y -= availableRange().y();
    }else{
        x -= boundingRect().x();
        y -= boundingRect().y();
    }
    m_selectRange = QRect(x, y, width, height);

    emit cursorChanged();
    update();
}
//有効領域
const QRect &ClipCursor::availableRange() const
{
    return m_availableRange;
}
//有効領域
void ClipCursor::setAvailableRange(const QRect &rect)
{
    if(m_availableRange == rect)
        return;

    m_availableRange = rect;
    emit availableRangeChanged();
}
//領域参照専用
const QRect &ClipCursor::selectRange() const
{
    return m_selectRange;
}
//自由選択
const bool &ClipCursor::freeDrag() const
{
    return m_freeDrag;
}
//自由選択
void ClipCursor::setFreeDrag(const bool &freedrag)
{
    if(m_freeDrag == freedrag)
        return;

    m_freeDrag = freedrag;
    emit freeDragChanged();
}

//描画処理
void ClipCursor::paint(QPainter *painter)
{
    if(!cursor().isValid() || !availableRange().isValid())
        return;

    qreal width = boundingRect().width();
    qreal height = boundingRect().height();

    painter->setRenderHint(QPainter::Antialiasing, antialiasing());

    //余白
    qreal c_left = cursor().left();
    qreal c_right = cursor().right();
    qreal c_top = cursor().top();
    qreal c_bottom = cursor().bottom();
    qreal c_width = cursor().width();
    painter->setPen(QPen(QColor(0, 0, 0, 0), 0));
    painter->setBrush(QBrush(QColor(0, 0, 0, 125)));
    painter->drawRect(QRect(0, 0, c_left, height));                             //左
    painter->drawRect(QRect(c_left, 0, c_width, c_top));                        //上
    painter->drawRect(QRect(c_right+1, 0, width - c_right-2, height));          //右
    painter->drawRect(QRect(c_left, c_bottom+1, c_width, height - c_bottom-2)); //下

    //カーソル
    painter->setPen(QPen(QColor(0, 200, 200, 255), 1));
    painter->setBrush(QBrush(QColor(0, 0, 0, 0)));
    painter->drawRect(cursor());
}

void ClipCursor::update(const QRect &rect)
{
    if(!m_delayUpdateTimer.isValid()){
//        qDebug() << "start " << m_delayUpdateTimer.elapsed();
        m_delayUpdateTimer.start();
    }else{
        if(m_delayUpdateTimer.hasExpired(33)){
            //時間過ぎてる
//            qDebug() << "timeout " << m_delayUpdateTimer.elapsed();
            m_delayUpdateTimer.start();
        }else{
            //まだ
            return;
        }
    }
    QQuickPaintedItem::update(rect);
}

void ClipCursor::mousePressEvent(QMouseEvent *event)
{
    m_prevPoint.setX(event->x());
    m_prevPoint.setY(event->y());

    if(freeDrag()){
        m_freeDragStartPoint.setX(event->x());
        m_freeDragStartPoint.setY(event->y());
        setCursor(QRect(event->x(), event->y(), 1, 1));
        setModifyTypeFromPosition(event->x(), event->y());
    }else{
        setMouseCursorFromPosition(event->x(), event->y());
    }
}

void ClipCursor::mouseReleaseEvent(QMouseEvent *event)
{
    modifyCursor(event->x(), event->y());

    if(freeDrag())
        setFreeDrag(false);

    QQuickPaintedItem::update();
}

void ClipCursor::mouseMoveEvent(QMouseEvent *event)
{
    if(freeDrag())
        setModifyTypeFromPosition(event->x(), event->y());

    modifyCursor(event->x(), event->y());

    m_prevPoint.setX(event->x());
    m_prevPoint.setY(event->y());
}

void ClipCursor::hoverEnterEvent(QHoverEvent *event)
{
    QQuickPaintedItem::setCursor(Qt::OpenHandCursor);
}

void ClipCursor::hoverLeaveEvent(QHoverEvent *event)
{
    QQuickPaintedItem::setCursor(Qt::ArrowCursor);
}

void ClipCursor::hoverMoveEvent(QHoverEvent *event)
{
    setMouseCursorFromPosition(event->pos().x(), event->pos().y());
}

//カーソルを位置で変更と移動orサイズ変更を切り替える
void ClipCursor::setMouseCursorFromPosition(qreal x, qreal y)
{
    qreal c_top = cursor().top();
    qreal c_bottom = cursor().bottom();
    qreal c_left = cursor().left();
    qreal c_right = cursor().right();

    //    qDebug() << "hover x,y=" << x << "," << y;

    if((x > (c_left - 10) && x < (c_left + 10)) && (y > (c_top - 10) && y < (c_top + 10))){
        //左上
        QQuickPaintedItem::setCursor(Qt::SizeFDiagCursor);
        m_modify = ModifyLeftTop;
    }else if((x > (c_left - 10) && x < (c_left + 10)) && (y > (c_bottom - 10) && y < (c_bottom + 10))){
        //左下
        QQuickPaintedItem::setCursor(Qt::SizeBDiagCursor);
        m_modify = ModifyLeftBottom;
    }else if((x > (c_right - 10) && x < (c_right + 10)) && (y > (c_top - 10) && y < (c_top + 10))){
        //右上
        QQuickPaintedItem::setCursor(Qt::SizeBDiagCursor);
        m_modify = ModifyRightTop;
    }else if((x > (c_right - 10) && x < (c_right + 10)) && (y > (c_bottom - 10) && y < (c_bottom + 10))){
        //右下
        QQuickPaintedItem::setCursor(Qt::SizeFDiagCursor);
        m_modify = ModifyRightBottom;
    }else if((x > (c_left - 10) && x < (c_left + 10)) && (y > c_top && y < c_bottom)){
        //左
        QQuickPaintedItem::setCursor(Qt::SizeHorCursor);
        m_modify = ModifyLeft;
    }else if((x > (c_right - 10) && x < (c_right + 10)) && (y > c_top && y < c_bottom)){
        //右
        QQuickPaintedItem::setCursor(Qt::SizeHorCursor);
        m_modify = ModifyRight;
    }else if((x > c_left && x < c_right) && (y > (c_top - 10) && y < (c_top + 10))){
        //上
        QQuickPaintedItem::setCursor(Qt::SizeVerCursor);
        m_modify = ModifyTop;
    }else if((x > c_left && x < c_right) && (y > (c_bottom - 10) && y < (c_bottom + 10))){
        //下
        QQuickPaintedItem::setCursor(Qt::SizeVerCursor);
        m_modify = ModifyBottom;
    }else if((x > c_left && x < c_right) && (y > c_top && y < c_bottom)){
        //範囲内
        QQuickPaintedItem::setCursor(Qt::OpenHandCursor);
        m_modify = MoveCursor;
    }else{
        //範囲外
        QQuickPaintedItem::setCursor(Qt::ArrowCursor);
        m_modify = NoModify;
    }
}

void ClipCursor::setModifyTypeFromPosition(qreal x, qreal y)
{
    qreal c_x = m_freeDragStartPoint.x();
    qreal c_y = m_freeDragStartPoint.y();

    if(c_x < x){
        //右
        if(c_y > y){
            //上
            m_modify = ModifyRightTop;
        }else{
            //下
            m_modify = ModifyRightBottom;
        }
    }else{
        //左
        if(c_y > y){
            //上
            m_modify = ModifyLeftTop;
        }else{
            //下
            m_modify = ModifyLeftBottom;
        }
    }
}

void ClipCursor::modifyCursor(qreal mouse_x, qreal mouse_y)
{
    if(m_modify == NoModify)
        return;

    qreal c_x = cursor().x();
    qreal c_y = cursor().y();
    qreal c_width = cursor().width();
    qreal c_height = cursor().height();
    qreal diff_x = mouse_x - m_prevPoint.x();
    qreal diff_y = mouse_y - m_prevPoint.y();

    qreal x = c_x;
    qreal y = c_y;
    qreal width = c_width;
    qreal height = c_height;

    switch(m_modify){
    case MoveCursor:
        x = c_x + diff_x;
        y = c_y + diff_y;
        break;

    case ModifyLeft:
        //左
        x = c_x + diff_x;
        width = c_width - diff_x;
        break;

    case ModifyTop:
        //上
        y = c_y + diff_y;
        height = c_height - diff_y;
        break;

    case ModifyRight:
        //右
        width = c_width + diff_x;
        break;

    case ModifyBottom:
        //下
        height = c_height + diff_y;
        break;

    case ModifyLeftTop:
        //左上
        x = c_x + diff_x;
        y = c_y + diff_y;
        width = c_width - diff_x;
        height = c_height - diff_y;
        break;

    case ModifyLeftBottom:
        //左下
        x = c_x + diff_x;
        width = c_width - diff_x;
        height = c_height + diff_y;
        break;

    case ModifyRightTop:
        //右上
        y = c_y + diff_y;
        width = c_width + diff_x;
        height = c_height - diff_y;
        break;

    case ModifyRightBottom:
        //右下
        width = c_width + diff_x;
        height = c_height + diff_y;
        break;

    default:
        break;
    }

    setCursor(QRect(x, y, width, height));
}
