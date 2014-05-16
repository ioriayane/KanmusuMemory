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

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);

    enum ButtleResultPosition {
        LeftTop = 0
        , RightTop = 1
        , LeftBottom = 2
        , RightBottom = 3
        , Center = 4
    };

    const QString &savePath() const;
    bool unusedTwitter() const;
    bool savePng() const;
    bool isMaskAdmiralName() const;
    bool isMaskHqLevel() const;
    bool isProxyEnable() const;
    const QString &proxyHost() const;
    quint16 proxyPort() const;
    bool useCookie() const;
    bool disableContextMenu() const;
    bool disableExitShortcut() const;
    bool viewButtleResult() const;
    ButtleResultPosition buttleResultPosition() const;
    qreal buttleResultOpacity() const;
    bool timerAutoStart() const;

    static QString selectSavePath(QWidget *parent, const QString &currentPath);

public slots:
    void setSavePath(const QString &savePath);
    void setUnusedTwitter(bool unusedTwitter);
    void setSavePng(bool savePng);
    void setMaskAdmiralName(bool maskAdmiralName);
    void setMaskHqLevel(bool maskHqLevel);
    void setProxyEnable(bool enable);
    void setProxyHost(const QString &proxyHost);
    void setProxyPort(quint16 proxyPort);
    void setUseCookie(bool use);
    void setDisableContextMenu(bool disable);
    void setDisableExitShortcut(bool disable);
    void setViewButtleResult(bool view);
    void setButtleResultPosition(ButtleResultPosition position);
    void setButtleResultOpacity(qreal opacity);
    void setTimerAutoStart(bool start);

signals:
    void savePathChanged(const QString &savePath);
    void unusedTwitterChanged(bool unusedTwitter);
    void savePngChanged(bool savePng);
    void maskAdmiralNameChanged(bool maskAdmiralName);
    void maskHqLevelChanged(bool maskHqLevel);
    void proxyEnableChanged(bool enable);
    void proxyHostChanged(const QString &proxyHost);
    void proxyPortChanged(quint16 proxyPort);
    void useCookieChanged(bool use);
    void disableContextMenuChanged(bool disable);
    void disableExitShortcutChanged(bool disable);
    void viewButtleResultChanged(bool view);
    void buttleResultPositionChanged(ButtleResultPosition position);
    void buttleResultOpacityChanged(qreal opacity);
    void timerAutoStartChanged(bool start);

private:
    class Private;
    Private *d;
};

#endif // SETTINGSDIALOG_H
