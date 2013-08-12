#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QFileDialog>
#include <QFile>

#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

const QString &SettingsDialog::savePath() const
{
    return m_savePath;
}

void SettingsDialog::setSavePath(const QString &save_path)
{
    m_savePath = save_path;
    ui->savePathEdit->setText(save_path);
}

QString SettingsDialog::selectSavePath(const QString &current_path)
{
    return QFileDialog::getExistingDirectory(NULL, tr("Select save folder"),
                                             current_path,
                                            QFileDialog::ShowDirsOnly
                                            | QFileDialog::DontResolveSymlinks);
}

void SettingsDialog::on_okButton_clicked()
{
    m_savePath = ui->savePathEdit->text();
    accept();
}

void SettingsDialog::on_cancelButton_clicked()
{
    reject();
}

//保存パスの参照ボタン
void SettingsDialog::on_selectPathButton_clicked()
{
    QString path = selectSavePath(ui->savePathEdit->text());

    qDebug() << path;

    if(QFile::exists(path))
        setSavePath(path);
}
