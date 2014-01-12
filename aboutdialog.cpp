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

class AboutDialog::Private
{
public:
    Private(AboutDialog *parent);

private:
    AboutDialog *q;
    Ui::AboutDialog ui;

public:
    QString version;
    QStringList developers;
};

AboutDialog::Private::Private(AboutDialog *parent)
    : q(parent)
{
    ui.setupUi(q);
    connect(q, &AboutDialog::versionChanged, [this](const QString &version) {
        ui.versionLabel->setText(QString("Version : %1").arg(version));
    });
    connect(q, &AboutDialog::developersChanged, [this](const QStringList &developers) {
        ui.developerLabel->setText(developers.join("\n"));
    });

    //Qtバージョン
    QString ver = "Based on Qt ";
    ver.append(QT_VERSION_STR);
    if(QT_POINTER_SIZE == 4){
        ver.append(" (32bit)");
    }else{
        ver.append(" (64bit)");
    }
    ui.qtVersionLabel->setText(ver);
}

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Private(this))
{
    connect(this, &QObject::destroyed, [this](){ delete d; });
}

const QString &AboutDialog::version() const
{
    return d->version;
}

void AboutDialog::setVersion(const QString &version)
{
    if (d->version == version) return;
    d->version = version;
    emit versionChanged(version);
}

const QStringList &AboutDialog::developers() const
{
    return d->developers;
}

void AboutDialog::setDevelopers(const QStringList &developers)
{
    if (d->developers == developers) return;
    d->developers = developers;
    emit developersChanged(developers);
}
