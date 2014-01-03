#ifndef FILELISTFILTERMODEL_H
#define FILELISTFILTERMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class FileListFilter
{
public:
    FileListFilter(const QString &filter
                   , const QString &filterName);

    QString filter() const;
    QString filterName() const;

private:
    QString m_filter;
    QString m_filterName;
};

class FileListFilterModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString folder READ folder WRITE setFolder NOTIFY folderChanged)
    Q_PROPERTY(QList<QString> nameFilters READ nameFilters WRITE setNameFilters NOTIFY nameFiltersChanged)

public:
    explicit FileListFilterModel(QObject *parent = 0);

    enum FileListFilterRoles {
        FilterRole = Qt::UserRole + 1
        ,FilterNameRole
    };

    Q_INVOKABLE void update();
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;


    QString folder() const;
    void setFolder(const QString &folder);
    QStringList nameFilters() const;
    void setNameFilters(const QStringList &nameFilters);

protected:
    QHash<int, QByteArray> roleNames() const;

signals:
    void folderChanged();
    void nameFiltersChanged();

public slots:


private:
    QList<FileListFilter> m_fileListFilter;
    QString m_folder;
    QStringList m_nameFilters;

    void parseDate(const QString &filename, QString *year, QString *month, QString *date);
};

#endif // FILELISTFILTERMODEL_H
