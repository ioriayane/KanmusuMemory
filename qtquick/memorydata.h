#ifndef MEMORYDATA_H
#define MEMORYDATA_H

#include <QObject>
#include <QString>

class MemoryData : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MemoryData)
    //プロパティを公開  [2]
    Q_PROPERTY(QString memoryPath READ memoryPath WRITE setMemoryPath NOTIFY memoryPathChanged)
    Q_PROPERTY(QString imagePath READ imagePath WRITE setImagePath NOTIFY imagePathChanged)
public:
    explicit MemoryData(QObject *parent = 0);

    const QString &memoryPath() const;
    void setMemoryPath(const QString &path);
    const QString &imagePath() const;
    void setImagePath(const QString &path);
    
signals:
    void memoryPathChanged(const QString &path);
    void imagePathChanged(const QString &path);

public slots:

private:
    QString m_memoryPath;  //表示パス
    QString m_imagePath;   //選択した結果の画像パス
};

#endif // MEMORYDATA_H
