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
#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    m_iconLabel(this),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    m_iconLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_iconLabel.setGeometry(QRect(20, 30, 80, 80));

    m_iconPixmap.load(":/resources/KanmusuMemory80.png");
    m_iconLabel.setPixmap(m_iconPixmap);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

QString AboutDialog::version() const
{
    return m_version;
}

void AboutDialog::setVersion(const QString &version)
{
    m_version = version;
    ui->versionLabel->setText(tr("Version : %1").arg(m_version));
}

QStringList AboutDialog::developers() const
{
    return m_developers;
}

void AboutDialog::setDevelopers(const QStringList &developers)
{
    m_developers = developers;

    QString dev;
    foreach (QString developer, developers) {
        dev.append(developer + "\n");
    }
    ui->developerLabel->setText(dev);
}
