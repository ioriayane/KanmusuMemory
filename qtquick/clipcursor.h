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
#ifndef CLIPCURSOR_H
#define CLIPCURSOR_H

#include <QQuickPaintedItem>
#include <QElapsedTimer>

class ClipCursor : public QQuickPaintedItem
{
    Q_OBJECT
public:
    enum ModifyType{
        NoModify
        , MoveCursor
        , ModifyLeft
        , ModifyTop
        , ModifyRight
        , ModifyBottom
        , ModifyLeftTop
        , ModifyLeftBottom
        , ModifyRightTop
        , ModifyRightBottom
    };

    explicit ClipCursor(QQuickPaintedItem *parent = 0);
    
    Q_PROPERTY(QRect cursor READ cursor WRITE setCursor NOTIFY cursorChanged)
    Q_PROPERTY(QRect availableRange READ availableRange WRITE setAvailableRange NOTIFY availableRangeChanged)
    Q_PROPERTY(QRect selectRange READ selectRange)
    Q_PROPERTY(bool freeDrag READ freeDrag WRITE setFreeDrag NOTIFY freeDragChanged)

    const QRect &cursor() const;
    void setCursor(const QRect &rect);
    const QRect &availableRange() const;
    void setAvailableRange(const QRect &rect);
    const QRect &selectRange() const;
    const bool &freeDrag() const;
    void setFreeDrag(const bool &freedrag);

    void paint(QPainter *painter);
    void update(const QRect &rect=QRect());

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void hoverEnterEvent(QHoverEvent *event);
    virtual void hoverLeaveEvent(QHoverEvent *event);
    virtual void hoverMoveEvent(QHoverEvent *event);

signals:
    void cursorChanged();
    void availableRangeChanged();
    void freeDragChanged();

public slots:

private:
    QRect m_cursor;
    QRect m_availableRange;
    QRect m_selectRange;

    QPoint m_prevPoint;
    QPoint m_freeDragStartPoint;
    ModifyType m_modify;
    bool m_freeDrag;

    QElapsedTimer m_delayUpdateTimer;

    void setMouseCursorFromPosition(qreal x, qreal y);
    void setModifyTypeFromPosition(qreal x, qreal y);
    void modifyCursor(qreal mouse_x, qreal mouse_y);
};

#endif // CLIPCURSOR_H
