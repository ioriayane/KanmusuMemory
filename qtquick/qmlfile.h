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
#ifndef QMLFILE_H
#define QMLFILE_H

#include <QObject>
#include <QTextStream>
#include <QStringList>
#include <QFile>
#include "qfileasync.h"

class QMLFile : public QObject
{
    Q_OBJECT

    Q_ENUMS(OperationStateType)
    Q_ENUMS(EntryType)
    Q_PROPERTY(OperationStateType operationState READ operationState WRITE setOperationState NOTIFY operationStateChanged)

public:
    enum OperationStateType{
        Idle
        , Copying
        , FinishedSuccess
        , FinishedFail
    };
    enum EntryType{
        Files
        , Dirs
    };

    //ファイルシステム系
    Q_INVOKABLE bool exists(const QString &path);
    Q_INVOKABLE QStringList getEntryList(const QString &path);
    Q_INVOKABLE QStringList getEntryList(const QString &path, const EntryType type, const QStringList &nameFilters);
    Q_INVOKABLE QString getExistingDirectory(const QString &title, const QString &default_path);
    Q_INVOKABLE QString getOpenFileName(const QString &title, const QString &default_path, const QString &filter);
    Q_INVOKABLE QString getApplicationPath();
    Q_INVOKABLE bool copy(const QString &fileName, const QString &newName);
    Q_INVOKABLE QString currentPath();
    Q_INVOKABLE bool setCurrent(const QString &path);
    Q_INVOKABLE QString fileName(const QString &path);
    Q_INVOKABLE QString filePath(const QString &path);
    Q_INVOKABLE QString fileBaseName(const QString &path);
    Q_INVOKABLE QString fileSuffix(const QString &path);

    //ファイル操作
    Q_INVOKABLE QString readTextFile(const QString &fileName);
    Q_INVOKABLE void writeTextFile(const QString &fileName, const QString &text);
    OperationStateType operationState() const;
    void setOperationState(const OperationStateType &state);

public:
    QMLFile(QObject *parent = 0);
    ~QMLFile();

signals:
    void operationStateChanged();

public slots:
    void asyncFinished(bool result);

private:
    OperationStateType m_operationState;        //コピー状態などなど

    QFileAsync _fileAcync;
    QFile _file;
    QFile *_pfile;
    QTextStream _textStream;

};

#endif // QMLFILE_H

