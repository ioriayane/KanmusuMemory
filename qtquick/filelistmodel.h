#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>
#include <QFileInfo>
#include <QStringList>

class FileListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString folder READ folder WRITE setFolder NOTIFY folderChanged)
    Q_PROPERTY(QList<QString> nameFilters READ nameFilters WRITE setNameFilters NOTIFY nameFiltersChanged)
    Q_PROPERTY(bool sortReversed READ sortReversed WRITE setSortReversed NOTIFY sortReversedChanged)

public:
    explicit FileListModel(QObject *parent = 0);

    enum FileListRoles {
        fileNameRole = Qt::UserRole + 1
        ,filePathRole
    };

    Q_INVOKABLE void update();
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    int count() const;
    QString folder() const;
    void setFolder(const QString &folder);
    QList<QString> nameFilters() const;
    void setNameFilters(const QList<QString> &nameFilters);
    bool sortReversed() const;
    void setSortReversed(bool sortReversed);

protected:
    QHash<int, QByteArray> roleNames() const;

signals:
    void countChanged();
    void folderChanged();
    void nameFiltersChanged();
    void sortReversedChanged();

public slots:

private:
    QFileInfoList m_fileInfoList;
    QString m_folder;
    QStringList m_nameFilters;
    bool m_sortReversed;
};

#endif // FILELISTMODEL_H
