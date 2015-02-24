#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent * ev);
    void mouseReleaseEvent(QMouseEvent * ev);
signals:
    void pressed(QMouseEvent *ev);
    void released(QMouseEvent *ev);

public slots:

};

#endif // CLICKABLELABEL_H
