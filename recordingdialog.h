#ifndef RECORDINGDIALOG_H
#define RECORDINGDIALOG_H

#include <QDialog>

namespace Ui {
class RecordingDialog;
}

class RecordingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecordingDialog(QWidget *parent = 0);
    ~RecordingDialog();

    int fps() const;
    QString audioSource() const;
    QString savePath() const;
    QString toolPath() const;
    QString tempPath() const;
    int soundOffset() const;
    bool dontViewButtleResult() const;
    bool muteNotificationSound() const;

    QString defaultSavePath() const;
    void setDefaultSavePath(const QString &defaultSavePath);
    QString defaultToolPath() const;
    void setDefaultToolPath(const QString &defaultToolPath);
    QString defaultTempPath() const;
    void setDefaultTempPath(const QString &defaultTempPath);
    int defaultFps() const;
    void setDefaultFps(int defaultFps);
    QString defaultAudioSource() const;
    void setDefaultAudioSource(const QString &defaultAudioSource);

    int defaultSoundOffset() const;
    void setDefaultSoundOffset(int defaultSoundOffset);

    bool defaultDontViewButtleResult() const;
    void setDefaultDontViewButtleResult(bool defaultDontViewButtleResult);
    bool defaultMuteNotificationSound() const;
    void setDefaultMuteNotificationSound(bool defaultMuteNotificationSound);


public slots:
    void setFps(int value);
    void setAudioSource(const QString &value);
    void setSavePath(const QString &value);
    void setToolPath(const QString &value);
    void setTempPath(const QString &value);
    void setSoundOffset(int offset);
    void setDontViewButtleResult(bool dontViewButtleResult);
    void setMuteNotificationSound(bool mute);

signals:
    void fpsChanged(int fps);
    void audioSourceChanged(const QString &source);
    void savePathChanged(const QString &path);
    void toolPathChanged(const QString &path);
    void tempPathChanged(const QString &path);
    void soundOffsetChanged(int offset);
    void dontViewButtleResultChanged(bool dont);
    void muteNotificationSoundChanged(bool mute);

private:
    class Private;
    Private *d;

    QString m_defaultSavePath;
    QString m_defaultToolPath;
    QString m_defaultTempPath;
    int m_defaultFps;
    QString m_defaultAudioSource;
    int m_defaultSoundOffset;
    bool m_defaultDontViewButtleResult;
    bool m_defaultMuteNotificationSound;
};

#endif // RECORDINGDIALOG_H
