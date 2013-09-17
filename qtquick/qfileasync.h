#ifndef QFILEASYNC_H
#define QFILEASYNC_H

#include <QThread>

class QFileAsync : public QThread
{
    Q_OBJECT
public:
    QFileAsync(QObject *parent = 0);

    enum OperationType{
        Copy
        , Move
        , Delete
    };

    OperationType m_Operation;
    QString m_fileName;
    QString m_newName;

signals:
    void finished(bool result);

public slots:
    
protected:
    void run();
};

#endif // QFILEASYNC_H
