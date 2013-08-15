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
