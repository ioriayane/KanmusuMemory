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
    Q_PROPERTY(OperatingSystemType type READ type)
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
    void pathPrefixChanged();

public slots:

};

#endif // OPERATINGSYSTEM_H
