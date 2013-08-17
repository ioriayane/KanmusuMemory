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
#include "operatingsystem.h"
#include <QDir>

OperatingSystem::OperatingSystem(QObject *parent) :
    QObject(parent)
{
}
//OS名を取得する
OperatingSystem::OperatingSystemType OperatingSystem::type() const
{
#if defined(Q_OS_WIN32)
    return Windows;
#elif defined(Q_OS_LINUX)
    return Linux;
#elif defined(Q_OS_MAC)
    return Mac;
#else
    return Other;
#endif
}
//パスの接頭辞を取得する
QString OperatingSystem::pathPrefix() const
{
    QString prefix;
    //Windowsのときだけ特殊
    if(type() == Windows){
        prefix = "file:///";
    }else{
        prefix = "file://";
    }
    return prefix;
}
//ユーザーのホームディレクトリを取得する
QString OperatingSystem::homeDirectory() const
{
    //QMLで使いやすい状態に加工
    return pathPrefix() + QDir::homePath() + "/";
}
