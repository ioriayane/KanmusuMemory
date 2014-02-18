#ifndef RECORDINGTHREAD_H
#define RECORDINGTHREAD_H

#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QProcess>
#include <QAudioRecorder>

#include "webview.h"


class SaveData
{
public:
    explicit SaveData(QImage img, QString path)
        : image(img), path(path)
    {
    }

    QImage image;
    QString path;
};

class RecordingThread : public QThread
{
    Q_OBJECT
public:
    explicit RecordingThread(QObject *parent = 0);

    enum RecordingState {
        Stop
        , Recording
        , Converting
    };

    void startRecording();
    void stopRecording();
    void clearCaptureFiles();

    WebView *webView() const;
    void setWebView(WebView *webView);
    QString savePath() const;
    void setSavePath(const QString &savePath);
    QString tempPath() const;
    void setTempPath(const QString &tempPath);
    QString toolPath() const;
    void setToolPath(const QString &toolPath);
    QString toolParam() const;
    void setToolParam(const QString &toolParam);
    qreal fps() const;
    void setFps(const qreal &fps);
    QString audioInputName() const;
    void setAudioInputName(const QString &audioInputName);
    QStringList audioInputNames() const;

signals:

public slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);
    void audioRecordingError(QMediaRecorder::Error error);
private:
    WebView *m_webView;
    QTimer *m_timer;
    unsigned long m_recordingCounter;
    QString m_savePath;                 //最終的に保存する動画ファイルのパス
    QString m_tempPath;                 //一時ファイルの保存場所。これが設定されてたらこっちを使用する。
    QString m_toolPath;                 //動画にするツールのパス
    QString m_toolParam;                //動画にするツールのパラメータ
    qreal m_fps;                        //フレームレート

    QProcess m_process;                 //動画にするツールを動かす
    QAudioRecorder m_audio;             //オーディオ録音
    QString m_audioInputName;           //オーディオ録音するデバイス名

    RecordingState m_state;             //状態

    QMutex m_mutex;
    QList<SaveData> m_SaveDataList;     //キャプチャしたデータをリストにしてスレッドでゆっくり保存

    void capture(unsigned long count);
    void convert();
    unsigned long getRecordingCounter();
    QString getTempPath();
protected:
    void run();
};

#endif // RECORDINGTHREAD_H
