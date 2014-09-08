#include "filelistfiltermodel.h"

#include <QDir>
#include <QDebug>

FileListFilter::FileListFilter(const QString &filter, const QString &filterName)
    : m_filter(filter),
      m_filterName(filterName)
{
}

QString FileListFilter::filter() const
{
    return m_filter;
}
QString FileListFilter::filterName() const
{
    return m_filterName;
}





FileListFilterModel::FileListFilterModel(QObject *parent) :
    QAbstractListModel(parent)
{
    update();
}

void FileListFilterModel::update()
{
//    qDebug() << folder() << nameFilters();

    if(folder().length() < 1)
        return;

    beginResetModel();
    m_fileListFilter.clear();
    endResetModel();

    QString year, pre_year;
    QString month, pre_month;
    QString date, pre_date;

    QDir dir(folder());
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    if(nameFilters().length() > 0)
        dir.setNameFilters(nameFilters());
    dir.setSorting(QDir::Name | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();
    pre_year = -1;
    pre_month = -1;
    pre_date = -1;
    for(int i = 0; i < list.size(); i++){
        QFileInfo fileInfo = list.at(i);
        parseDate(fileInfo.fileName(), &year, &month, &date);

        if(year != pre_year || month != pre_month || date != pre_date){
//            qDebug() << fileInfo.fileName() << "," << year << month << date;
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            m_fileListFilter.append(FileListFilter(QString("shiromusu_%1-%2-%3*").arg(year).arg(month).arg(date)
                                                   , QString("%1/%2/%3").arg(year).arg(month).arg(date)));
            endInsertRows();
        }

        pre_year = year;
        pre_month = month;
        pre_date = date;
    }

}

int FileListFilterModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_fileListFilter.count();
}

QVariant FileListFilterModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_fileListFilter.count())
        return QVariant();

    const FileListFilter &filter = m_fileListFilter[index.row()];
    if (role == FilterRole)
        return filter.filter();
    else if(role == FilterNameRole)
        return filter.filterName();

    return QVariant();
}

//対象フォルダ
QString FileListFilterModel::folder() const
{
    return m_folder;
}
void FileListFilterModel::setFolder(const QString &folder)
{
    if(folder == m_folder)
        return;

    m_folder = folder;
    update();
    emit folderChanged();
}
QStringList FileListFilterModel::nameFilters() const
{
    return m_nameFilters;
}

void FileListFilterModel::setNameFilters(const QStringList &nameFilters)
{
    if(m_nameFilters == nameFilters)
        return;

    m_nameFilters = nameFilters;
//    update();
    emit nameFiltersChanged();
}

QHash<int, QByteArray> FileListFilterModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FilterRole] = "filter";
    roles[FilterNameRole] = "filterName";
    return roles;
}



void FileListFilterModel::parseDate(const QString &filename, QString *year, QString *month, QString *date)
{
    QStringList temp = filename.split(QStringLiteral("_"));
    year->clear();
    month->clear();
    date->clear();
    if(temp.length() > 1){
        temp = temp[1].split(QStringLiteral("-"));
        if(temp.length() > 2){
            year->append(temp[0]);
            month->append(temp[1]);
            date->append(temp[2]);
        }
    }
}
