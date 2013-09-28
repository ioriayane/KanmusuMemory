#ifndef WEBPAGEFORM_H
#define WEBPAGEFORM_H

#include <QWidget>
#include <QtCore/QUrl>

namespace Ui {
class WebPageForm;
}

class WebPageForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit WebPageForm(QWidget *parent = 0);
    ~WebPageForm();

    
    QUrl url() const;
    void setUrl(const QUrl &url);
    QString title() const;

signals:
    void urlChanged();

private:
    class Private;
    Private *d;

    QUrl m_url;
};

#endif // WEBPAGEFORM_H
