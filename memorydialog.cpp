#include "memorydialog.h"
#include "ui_memorydialog.h"
#include "thumbnailprovider.h"

#include <QQmlContext>
#include <QtQml/QQmlEngine>

MemoryDialog::MemoryDialog(QWidget *parent) :
    QDialog(parent),
    m_viewer(NULL),
    ui(new Ui::MemoryDialog)
{
    ui->setupUi(this);

}

MemoryDialog::~MemoryDialog()
{
    delete ui;
}

void MemoryDialog::setQmlSource(const QUrl &url)
{
    m_qmlUrl = url;
}

void MemoryDialog::setMemoryPath(const QString &path)
{
    m_memoryPath = path;
}

const QString &MemoryDialog::imagePath()
{
    return m_imagePath;
}

void MemoryDialog::resizeEvent(QResizeEvent *event)
{
    if(m_viewer != NULL){
        m_viewer->setGeometry(QRect(0, 0, width(), height()));
    }
}

void MemoryDialog::showEvent(QShowEvent *event)
{
    if(m_viewer == NULL){
        m_viewer = new QtQuick2ApplicationViewer(windowHandle());
        connect(m_viewer->engine(), SIGNAL(quit()), this, SLOT(closeQml()));
        //プラグインを登録
        m_viewer->engine()->addImageProvider(QStringLiteral("thumbnail")
                                          , new ThumbnailProvider);
        m_viewer->setSource(m_qmlUrl);
        m_viewer->show();

        m_data.setMemoryPath(m_memoryPath);
        m_viewer->rootContext()->setContextProperty("memoryData", &m_data);
    }
}

void MemoryDialog::closeQml()
{
    m_imagePath = m_data.imagePath();
    accept();
}
