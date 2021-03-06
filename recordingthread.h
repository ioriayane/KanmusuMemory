#ifndef RECORDINGTHREAD_H
#define RECORDINGTHREAD_H

#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QProcess>
//#include <QAudioRecorder>
#include <QElapsedTimer>

#include "webview.h"
#include "audiorecorder.h"

class SaveData
{
public:
    explicit SaveData(QImage img, QString path, qint64 elapse, qint64 duration)
        : image(img), path(path), elapse(elapse), duration(duration)
    {
    }

    QImage image;
    QString path;   //これいらないかも
    qint64 elapse;
    qint64 duration;    //audio
};

class RecordingThread : public QThread
{
    Q_OBJECT
public:
    explicit RecordingThread(QObject *parent = 0);
    ~RecordingThread();

    enum RecordingStatus {
        Ready
//        , Stop
//        , Wait
        , Recording
        , Saving
        , Convert
    };

    void startRecording();
    void stopRecording();
    void clearCaptureFiles();
    bool isSettingValid();

    QString getTempPath();

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

    RecordingStatus status() const;
    void setStatus(const RecordingStatus &status);
    qint64 duration() const;
    void setDuration(const qint64 &duration);

    qint32 soundOffset() const;
    void setSoundOffset(const qint32 &soundOffset);

signals:
    void audioRecord(const QUrl& dest, const QString& src);
    //    void audioRecordStop();
    void statusChanged(RecordingStatus status);
    void durationChanged(const qint64 &duration);

public slots:
    void recordFinished();
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
    qint32 m_soundOffset;               //音ズレ調整用のフレーム数
                                        //  正：動画に対して音が後ろへ行く。つまり動画の最初を捨てる
                                        //  負：動画に対して音が前へ行く。つまり動画の頭が水増しされる
    qint32 m_soundOffsetCount;          //どれくらい音ズレ調整したかのカウンタ

    QProcess m_process;                 //動画にするツールを動かす
    QString m_audioInputName;           //オーディオ録音するデバイス名
    AudioRecorder m_audio;              //オーディオ録音
    QThread m_recordThread;             //録音を別スレッドにする

    qint64 m_start_offset_time;         //計測開始から1枚目の画像までの時間
    qint64 m_duration;                  //経過時間

    RecordingStatus m_status;            //スレッドの状態

    bool m_stop;
    QMutex m_mutex;
    QList<SaveData> m_SaveDataList;     //キャプチャしたデータをリストにしてスレッドでゆっくり保存

    QElapsedTimer m_et;                 //デバッグ計測用
    QList<qint64> m_interval;           //デバッグ計測用

    void capture(unsigned long count);
    void convert();
    unsigned long getRecordingCounter();
    QString getTempAudioPath();

protected:
    void run();

private:
    //以下スレッドから使う
    void save(SaveData &data, unsigned long count);
};

#endif // RECORDINGTHREAD_H
