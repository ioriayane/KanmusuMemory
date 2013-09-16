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
#ifndef OPERATINGSYSTEM_H
#define OPERATINGSYSTEM_H

#include <QQuickItem>

class OperatingSystem : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(OperatingSystem)
    //列挙型の公開
    Q_ENUMS(OperatingSystemType)
    //プロパティを公開
    Q_PROPERTY(OperatingSystemType type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString pathPrefix READ pathPrefix NOTIFY pathPrefixChanged)
    Q_PROPERTY(QString homeDirectory READ homeDirectory)

public:
    explicit OperatingSystem(QObject *parent = 0);

    enum OperatingSystemType{
        Windows
        , Linux
        , Mac
        , Other
    };
    //プロパティの参照
    OperatingSystemType type() const;
    QString pathPrefix() const;
    QString homeDirectory() const;

signals:
    void typeChanged();
    void pathPrefixChanged();

public slots:

};

#endif // OPERATINGSYSTEM_H
