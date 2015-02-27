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
