#ifndef WEBPAGEOPERATION_H
#define WEBPAGEOPERATION_H

#include <QtWidgets>
#include <QWebFrame>


class WebStyle : public QObject
{
    Q_OBJECT
public:
    explicit WebStyle(QObject *parent = 0){}

    void append(const QString &name, const QString &value){
        m_name << name;
        m_value << value;
    }

    const bool isEmpty() const {
        if(m_name.length() > 0)
            return false;
        else
            return true;
    }
    const int length() const {
        return m_name.length();
    }
    const QString &name(int i) const {
        if(i >= m_name.length())
            return "";
        return m_name[i];
    }
    const QString &value(int i) const {
        if(i >= m_value.length())
            return "";
        return m_value[i];
    }

private:
    QStringList m_name;
    QStringList m_value;
};

class WebPageOperation : public QWidget
{
    Q_OBJECT
public:
    explicit WebPageOperation(QWidget *parent = 0);
    
    void fullScreen(bool isFull);
    void setMainFrame(QWebFrame *mainFrame);

signals:
    
public slots:
    
private:
    QWidget *q;
    QWebFrame *m_mainFrame;

    WebStyle m_body;
    WebStyle m_gameFrame;
    WebStyle m_flashWrap;
    WebStyle m_embed;
    WebStyle m_sectionWrap;
};

#endif // WEBPAGEOPERATION_H
