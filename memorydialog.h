#ifndef MEMORYDIALOG_H
#define MEMORYDIALOG_H

#include <QDialog>
#include <QUrl>
#include "qtquick2applicationviewer.h"
#include "memorydata.h"

namespace Ui {
class MemoryDialog;
}

class MemoryDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MemoryDialog(QWidget *parent = 0);
    ~MemoryDialog();
    
    void setQmlSource(const QUrl &url);
    void setMemoryPath(const QString &path);
    const QString &imagePath();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent *event);

public slots:
    void closeQml();

private:
    Ui::MemoryDialog *ui;

    QtQuick2ApplicationViewer *m_viewer;
    QUrl m_qmlUrl;
    MemoryData m_data;
    QString m_memoryPath;   //保存パス
    QString m_imagePath;    //つぶやく対象の画像
};

#endif // MEMORYDIALOG_H
