/*
 * Copyright 2013 KanMemo Project.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QMessageBox>
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

QString SettingsDialog::selectSavePath(QWidget *parent, const QString &current_path)
{
    return QFileDialog::getExistingDirectory(parent, tr("Select save folder"),
                                             current_path,
                                             QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);
}

bool SettingsDialog::unusedTwitter() const
{
    return m_unusedTwitter;
}

void SettingsDialog::setUnusedTwitter(bool unusedTwitter)
{
    m_unusedTwitter = unusedTwitter;
    ui->unusedTwittercheckBox->setChecked(unusedTwitter);
}

bool SettingsDialog::savePng() const
{
    return m_savePng;
}
void SettingsDialog::setSavePng(bool savePng)
{
    m_savePng = savePng;
    ui->savePngCheckBox->setChecked(savePng);
}

bool SettingsDialog::isMaskAdmiralName() const
{
    return m_maskAdmiralName;
}
void SettingsDialog::setMaskAdmiralName(bool b)
{
    m_maskAdmiralName = b;
    ui->maskAdmiralNameCheckBox->setChecked(b);
}

bool SettingsDialog::isMaskHqLevel() const
{
    return m_maskHqLevel;
}
void SettingsDialog::setMaskHqLevel(bool b)
{
    m_maskHqLevel = b;
    ui->maskHqLevelCheckBox->setChecked(b);
}


void SettingsDialog::on_okButton_clicked()
{
    if(!QFile::exists(ui->savePathEdit->text())){
        QMessageBox::warning(this, "warning", "save path not found.");
        return;
    }
    
    m_savePath = ui->savePathEdit->text();
    m_unusedTwitter = ui->unusedTwittercheckBox->isChecked();
    m_savePng = ui->savePngCheckBox->isChecked();
    m_maskAdmiralName = ui->maskAdmiralNameCheckBox->isChecked();
    m_maskHqLevel = ui->maskHqLevelCheckBox->isChecked();

    accept();
}

void SettingsDialog::on_cancelButton_clicked()
{
    reject();
}

//保存パスの参照ボタン
void SettingsDialog::on_selectPathButton_clicked()
{
    QString path = selectSavePath(this, ui->savePathEdit->text());
    
    if(QFile::exists(path))
        setSavePath(path);
}
