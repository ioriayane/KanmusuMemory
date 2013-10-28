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

class SettingsDialog::Private
{
public:
    Private(SettingsDialog *parent);

private:
    SettingsDialog *q;
    Ui::SettingsDialog ui;

public:
    QString savePath;
    bool unusedTwitter;
    bool savePng;
    bool maskAdmiralName;
    bool maskHqLevel;
    bool proxyEnable;
    QString proxyHost;
    quint16 proxyPort;
};

SettingsDialog::Private::Private(SettingsDialog *parent)
    : q(parent)
{
    ui.setupUi(q);

    connect(ui.buttonBox, &QDialogButtonBox::accepted, [this](){
        if(!QFile::exists(ui.savePathEdit->text())){
            QMessageBox::warning(q, "warning", "save path not found.");
            return;
        }

        savePath = ui.savePathEdit->text();
        unusedTwitter = ui.unusedTwittercheckBox->isChecked();
        savePng = ui.savePngCheckBox->isChecked();
        maskAdmiralName = ui.maskAdmiralNameCheckBox->isChecked();
        maskHqLevel = ui.maskHqLevelCheckBox->isChecked();
        proxyEnable = ui.proxyEnableCheckBox->isChecked();
        proxyHost = ui.proxyHost->text();
        proxyPort = ui.proxyPort->value();
        q->accept();
    });
    connect(ui.buttonBox, &QDialogButtonBox::rejected, q, &QDialog::reject);

    //保存パスの参照ボタン
    connect(ui.selectPathButton, &QToolButton::clicked, [this]() {
        QString path = q->selectSavePath(q, ui.savePathEdit->text());

        if(QFile::exists(path))
            q->setSavePath(path);
    });

    connect(q, &SettingsDialog::savePathChanged, ui.savePathEdit, &QLineEdit::setText);
    connect(q, &SettingsDialog::unusedTwitterChanged, ui.unusedTwittercheckBox, &QCheckBox::setChecked);
    connect(q, &SettingsDialog::savePngChanged, ui.savePngCheckBox, &QCheckBox::setChecked);
    connect(q, &SettingsDialog::maskAdmiralNameChanged, ui.maskAdmiralNameCheckBox, &QCheckBox::setChecked);
    connect(q, &SettingsDialog::maskHqLevelChanged, ui.maskHqLevelCheckBox, &QCheckBox::setChecked);
    connect(q, &SettingsDialog::proxyEnableChanged, ui.proxyEnableCheckBox, &QCheckBox::setChecked);
    connect(q, &SettingsDialog::proxyHostChanged, ui.proxyHost, &QLineEdit::setText);
    connect(q, &SettingsDialog::proxyPortChanged, ui.proxyPort, &QSpinBox::setValue);

    QSpinBox * portNum = ui.proxyPort;
    portNum->setMinimum(1);
    portNum->setMaximum(65535);
}

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Private(this))
{
    connect(this, &QObject::destroyed, [this]() { delete d; });
}

const QString &SettingsDialog::savePath() const
{
    return d->savePath;
}

void SettingsDialog::setSavePath(const QString &savePath)
{
    if (d->savePath == savePath) return;
    d->savePath = savePath;
    emit savePathChanged(savePath);
}

QString SettingsDialog::selectSavePath(QWidget *parent, const QString &currentPath)
{
    return QFileDialog::getExistingDirectory(parent, tr("Select save folder"),
                                             currentPath,
                                             QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);
}

bool SettingsDialog::unusedTwitter() const
{
    return d->unusedTwitter;
}

void SettingsDialog::setUnusedTwitter(bool unusedTwitter)
{
    if (d->unusedTwitter == unusedTwitter) return;
    d->unusedTwitter = unusedTwitter;
    emit unusedTwitterChanged(unusedTwitter);
}

bool SettingsDialog::savePng() const
{
    return d->savePng;
}

void SettingsDialog::setSavePng(bool savePng)
{
    if (d->savePng == savePng) return;
    d->savePng = savePng;
    emit savePngChanged(savePng);
}

bool SettingsDialog::isMaskAdmiralName() const
{
    return d->maskAdmiralName;
}

void SettingsDialog::setMaskAdmiralName(bool maskAdmiralName)
{
    if (d->maskAdmiralName == maskAdmiralName) return;
    d->maskAdmiralName = maskAdmiralName;
    emit maskAdmiralNameChanged(maskAdmiralName);
}

bool SettingsDialog::isMaskHqLevel() const
{
    return d->maskHqLevel;
}

bool SettingsDialog::isProxyEnable() const
{
    return d->proxyEnable;
}

void SettingsDialog::setMaskHqLevel(bool maskHqLevel)
{
    if (d->maskHqLevel == maskHqLevel) return;
    d->maskHqLevel = maskHqLevel;
    emit maskHqLevelChanged(maskHqLevel);
}

void SettingsDialog::setProxyEnable(bool enable)
{
    if(d->proxyEnable == enable) return;
    d->proxyEnable = enable;
    emit proxyEnableChanged(enable);
}

const QString &SettingsDialog::proxyHost() const
{
    return d->proxyHost;
}

void SettingsDialog::setProxyHost(const QString &proxyHost)
{
    if(d->proxyHost == proxyHost) return;
    d->proxyHost = proxyHost;
    emit proxyHostChanged(proxyHost);
}

quint16 SettingsDialog::proxyPort() const
{
    return d->proxyPort;
}

void SettingsDialog::setProxyPort(quint16 proxyPort)
{
    if(d->proxyPort == proxyPort) return;
    d->proxyPort = proxyPort;
    emit proxyPortChanged(proxyPort);
}
