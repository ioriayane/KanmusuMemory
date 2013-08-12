#include "mainwindow.h"
#include <QApplication>
#include <QtCore/QTranslator>
#include <QtCore/QLocale>

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if defined(Q_OS_MAC)
    //Mac OS X 用のパスを作成する
    QString dir = QString("%1/i18n")
            .arg(QCoreApplication::applicationDirPath());
#else
    //Windows とLinux 用のパスを作成する
    QString dir("i18n");
#endif
    QTranslator translator;
    //言語ファイルの読み込み
    translator.load(QString("qt_%1" ).arg(QLocale::system().name()), dir);
    //言語データを登録する
    a.installTranslator(&translator);

    MainWindow w;
    w.show();
    
    return a.exec();
}
