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
#include "qmlfile.h"

#include <QDir>
#include <QFileDialog>
#include <QApplication>
#include <QStandardPaths>

QMLFile::QMLFile(QObject *parent):
    QObject(parent)
{
    // By default, QDeclarativeItem does not draw anything. If you subclass
    // QDeclarativeItem to create a visual item, you will need to uncomment the
    // following line:
    
    // setFlag(ItemHasNoContents, false);

    m_operationState = Idle;
    _pfile = NULL;

    connect(&_fileAcync, SIGNAL(finished(bool)), this, SLOT(asyncFinished(bool)));
}

QMLFile::~QMLFile()
{
}

//存在確認
bool QMLFile::exists(const QString &path)
{
    return QFile::exists(path);
}

//ディレクトリ内のディレクトリリスト
QStringList QMLFile::getEntryList(const QString &path)
{
    QStringList list;
    return getEntryList(path, Dirs, list);
}

QStringList QMLFile::getEntryList(const QString &path, const EntryType type, const QStringList &nameFilters)
{
    QStringList ret;
    QDir dir(path);
    if(type == Dirs){
        dir.setFilter(QDir::Dirs | QDir::Hidden | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        if(nameFilters.length() > 0){
            dir.setNameFilters(nameFilters);
        }
    }else{
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        dir.setNameFilters(nameFilters);
    }
    dir.setSorting(QDir::Name);// | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        ret.append(fileInfo.fileName());
    }
    return ret;
}

//フォルダ選択ダイアログ
QString QMLFile::getExistingDirectory(const QString &title, const QString &default_path)
{
    return QFileDialog::getExistingDirectory(NULL, title,
                                             default_path,
                                            QFileDialog::ShowDirsOnly
                                            | QFileDialog::DontResolveSymlinks);
}
//ファイル選択ダイアログ
QString QMLFile::getOpenFileName(const QString &title, const QString &default_path, const QString &filter)
{
    return QFileDialog::getOpenFileName(NULL, title
                                        , default_path
                                        , filter);
}

QString QMLFile::getApplicationPath()
{
    return QApplication::applicationDirPath();
}

QString QMLFile::getWritablePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
}


//ファイルコピー開始
bool QMLFile::copy(const QString &fileName, const QString &newName)
{
    bool ret = false;
    if(m_operationState != Idle){
    }else if(fileName.length() < 1 || newName.length() < 1){
    }else{
        _fileAcync.m_fileName = fileName;
        _fileAcync.m_newName = newName;
        _fileAcync.start();
        ret = true;
    }
    return ret;
}

//カレントパスを取得
QString QMLFile::currentPath()
{
    return QDir::currentPath();
}
//カレントパスを設定
bool QMLFile::setCurrent(const QString &path)
{
    return QDir::setCurrent(path);
}
//ファイル名取得
QString QMLFile::fileName(const QString &path)
{
    QFileInfo fi(path);
    return fi.fileName();
}
//パス取得
QString QMLFile::filePath(const QString &path)
{
    QFileInfo fi(path);
    return fi.path();
}
//ファイル名（拡張子なし）を取得
QString QMLFile::fileBaseName(const QString &path)
{
    QFileInfo fi(path);
    return fi.completeBaseName();
}
//拡張子を取得
QString QMLFile::fileSuffix(const QString &path)
{
    QFileInfo fi(path);
    return fi.completeSuffix();
}

/////////////////////////////////////////////////////////////
// ファイル操作
/////////////////////////////////////////////////////////////

QString QMLFile::readTextFile(const QString &fileName)
{
    QString str;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
    }else{
        QTextStream in(&file);
        in.setCodec("UTF-8");
        while(!in.atEnd()){
            str.append(in.readLine());
        }
        file.close();
    }
    return str;
}

void QMLFile::writeTextFile(const QString &fileName, const QString &text)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
    }else{
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << text;
        file.close();
    }
}

/////////////////////////////////////////////////////////////
// setter/getter
/////////////////////////////////////////////////////////////


//コピーなどの状態ステータス
QMLFile::OperationStateType QMLFile::operationState() const
{
    return m_operationState;
}
void QMLFile::setOperationState(const OperationStateType &state)
{
    if(state != m_operationState){
        m_operationState = state;
        emit operationStateChanged();
    }
}

//非同期処理の応答スロット
void QMLFile::asyncFinished(bool result)
{
    m_operationState = result ? FinishedSuccess : FinishedFail;
    emit operationStateChanged();
    m_operationState = Idle;
}
