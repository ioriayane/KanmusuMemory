#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent) :
    QLabel(parent)
{
}

void ClickableLabel::mousePressEvent(QMouseEvent *ev)
{
    emit pressed(ev);
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    emit released(ev);
}
