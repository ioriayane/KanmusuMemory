#include "filelistmodel.h"

#include <QtCore/QDir>
#include <QDebug>

FileListModel::FileListModel(QObject *parent) :
    QAbstractListModel(parent)
  , m_sortReversed(true)
{
}

void FileListModel::update()
{
//    qDebug() << "update:" << folder() << nameFilters();

    if(folder().length() < 1)
        return;

    beginResetModel();
    m_fileInfoList.clear();
    endResetModel();

    QDir dir(folder());
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    if(nameFilters().length() > 0)
        dir.setNameFilters(nameFilters());
    if(sortReversed())
        dir.setSorting(QDir::Name | QDir::Reversed);
    else
        dir.setSorting(QDir::Name);

    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i < list.size(); i++){
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_fileInfoList.append(list.at(i));
        endInsertRows();
    }
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_fileInfoList.count();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_fileInfoList.count())
        return QVariant();

    const QFileInfo &fileinfo = m_fileInfoList[index.row()];
    if(role == fileNameRole)
        return fileinfo.fileName();
    else if(role == filePathRole)
        return fileinfo.filePath();

    return QVariant();
}

int FileListModel::count() const
{
    return m_fileInfoList.count();
}
QString FileListModel::folder() const
{
    return m_folder;
}
void FileListModel::setFolder(const QString &folder)
{
    if(m_folder == folder)
        return;

    m_folder = folder;
    update();
    emit folderChanged();
}
QList<QString> FileListModel::nameFilters() const
{
    return m_nameFilters;
}
void FileListModel::setNameFilters(const QList<QString> &nameFilters)
{
    if(m_nameFilters == nameFilters)
        return;

    m_nameFilters = nameFilters;
//    update();
    emit nameFiltersChanged();
}
bool FileListModel::sortReversed() const
{
    return m_sortReversed;
}
void FileListModel::setSortReversed(bool sortReversed)
{
    if(m_sortReversed == sortReversed)
        return;
    m_sortReversed = sortReversed;
//    update();
    emit sortReversedChanged();
}

QHash<int, QByteArray> FileListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[fileNameRole] = "fileName";
    roles[filePathRole] = "filePath";
    return roles;
}



