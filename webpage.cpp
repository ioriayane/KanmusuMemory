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
#include "webpage.h"

WebPage::WebPage(QObject *parent)
    :QWebPage(parent),
      m_isMoibleMode(NULL)
{
    this->parent = parent;
}

//USER AGENTを調節
QString WebPage::userAgentForUrl(const QUrl &url) const
{
    QString ret = QWebPage::userAgentForUrl(url);
    if(isMoibleMode()){
#if defined(Q_OS_WIN)
        //            ret = QStringLiteral("Mozilla/5.0 (Windows Phone OS 7.5;) AppleWebKit/537.21 (KHTML, like Gecko) ShiromusuMemory Safari/537.21");
        ret = QStringLiteral("Mozilla/5.0 (Linux; U; Android 4.1.1; ja-jp;) AppleWebKit/537.21 (KHTML, like Gecko) ShiromusuMemory Safari/537.21");
#elif defined(Q_OS_LINUX)
        ret = QStringLiteral("Mozilla/5.0 (Linux; U; Android 4.1.1; ja-jp;) AppleWebKit/537.21 (KHTML, like Gecko) ShiromusuMemory Safari/537.21");
#elif defined(Q_OS_MAC)
        ret = QStringLiteral("Mozilla/5.0 (iPhone; CPU iPhone OS 6_0 like Mac OS X) AppleWebKit/537.21 (KHTML, like Gecko) ShiromusuMemory Safari/537.21");
#endif
    }
    return ret;
    //        return QString("Mozilla/5.0 (Windows NT 6.2; Mobile; WOW64) AppleWebKit/537.21 (KHTML, like Gecko) KanmusuMemory Safari/537.21");
    //        return QString("Mozilla/5.0 (Linux; U; Android 4.1.1; ja-jp; Galaxy Nexus Build/JRO03H) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30");
}

QWebPage *WebPage::createWindow(QWebPage::WebWindowType type)
{
    Q_UNUSED(type);
    WebPage *webpage = new WebPage(parent);
    emit createNewWindow(webpage);
    return webpage;
}

bool WebPage::isMoibleMode() const
{
    if(m_isMoibleMode == NULL){
        return false;
    }else{
        return *m_isMoibleMode;
    }
}

void WebPage::setIsMoibleMode(bool *isMoibleMode)
{
    m_isMoibleMode = isMoibleMode;
}

