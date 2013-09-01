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
#ifndef WEBPAGEOPERATION_H
#define WEBPAGEOPERATION_H

#include <QtWidgets>
#include <QWebView>


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
    
    bool existGame();
    QRect getGameRect();
    void fullScreen(bool isFull);
    void setWebView(QWebView *value);

signals:
    
public slots:
    
private:
    QWidget *q;
    QWebView *m_webView;

    WebStyle m_body;
    WebStyle m_gameFrame;
    WebStyle m_flashWrap;
    WebStyle m_embed;
    WebStyle m_sectionWrap;
};

#endif // WEBPAGEOPERATION_H
