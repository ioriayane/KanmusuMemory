#include "qfileasync.h"
#include <QDebug>
#include <QFile>

QFileAsync::QFileAsync(QObject *parent) :
    QThread(parent)
{
}


void QFileAsync::run()
{
    bool ret = false;

    qDebug() << "run file ope.";

    switch(m_Operation){
    case Copy:
        ret = QFile::copy(m_fileName, m_newName);
        break;

    case Move:
        ret = QFile::copy(m_fileName, m_newName);
        if(ret){
            ret = QFile::remove(m_fileName);
        }
        break;

    case Delete:
        ret = QFile::remove(m_fileName);
        break;

    default:
        break;
    }

    emit finished(ret);
}
