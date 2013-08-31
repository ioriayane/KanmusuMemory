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
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class AboutDialog : public QDialog
{
    Q_OBJECT    
public:
    explicit AboutDialog(QWidget *parent = 0);
    
    const QString &version() const;
    const QStringList &developers() const;

public slots:
    void setVersion(const QString &version);
    void setDevelopers(const QStringList &developers);

signals:
    void versionChanged(const QString &version);
    void developersChanged(const QStringList &developers);

private:
    class Private;
    Private *d;
};

#endif // ABOUTDIALOG_H
