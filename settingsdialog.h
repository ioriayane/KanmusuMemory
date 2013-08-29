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
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    
    const QString &savePath() const;
    void setSavePath(const QString &save_path);

    static QString selectSavePath(QWidget *parent, const QString &current_path);

    bool unusedTwitter() const;
    void setUnusedTwitter(bool unusedTwitter);

    bool savePng() const;
    void setSavePng(bool savePng);

    bool isMaskAdmiralName() const;
    void setMaskAdmiralName(bool b);

    bool isMaskHqLevel() const;
    void setMaskHqLevel(bool b);

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_selectPathButton_clicked();

private:
    Ui::SettingsDialog *ui;

    QString m_savePath;
    bool m_unusedTwitter;
    bool m_savePng;
    bool m_maskAdmiralName;
    bool m_maskHqLevel;
};

#endif // SETTINGSDIALOG_H
