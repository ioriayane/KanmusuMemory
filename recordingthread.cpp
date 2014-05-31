#include "recordingthread.h"

#include <QStandardPaths>
#include <QDir>
#include <QtCore/QDebug>

#include "kanmusumemory_global.h"

RecordingThread::RecordingThread(QObject *parent) :
    QThread(parent)
  , m_webView(NULL)
  , m_timer(NULL)
  , m_recordingCounter(0)
  , m_audio()
  , m_recordThread(this)
  , m_stop(true)
{
    //録音
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/pcm");
    audioSettings.setQuality(QMultimedia::HighQuality);
    m_audio.setEncodingSettings(audioSettings);
    m_audio.moveToThread(&m_recordThread);

    //テンポラリ
    setTempPath(QString("%1/%2/%3")
                .arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                .arg(KANMEMO_PROJECT)
                .arg("recording"));

    //タイマー
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [this](){
        static bool f = true;
        if(f){
            qDebug() << "timer first " << m_et.elapsed();
            f = false;
        }

        //キャプチャ
        unsigned long counter = getRecordingCounter();
        capture(counter);

        //保存スレッド開始
        if(!isRunning()){
            start();
        }
    });

    //スレッド終了
    connect(this, &QThread::finished, [this](){
//        qDebug() << "finish thread " << m_state;
    });

    //プロセスが終了した
    connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)) );
    //プロセスがエラー
    connect(&m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)) );

    //録音開始
    connect(this, &RecordingThread::audioRecord, &m_audio, &AudioRecorder::record);
    //録音がエラー
    connect(&m_audio, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(audioRecordingError(QMediaRecorder::Error)));
    //録音の状態変化
    connect(&m_audio, &QAudioRecorder::statusChanged, [this](QMediaRecorder::Status status){
        qDebug() << "QAudioRecorder::statusChanged " << status << "," << QString::number(m_et.elapsed()) << "," << m_audio.duration();
    });


    //録音スレッド開始
    m_recordThread.start(QThread::TimeCriticalPriority);
//    m_recordThread.setPriority(QThread::TimeCriticalPriority);

    //状態変更
    setStatus(Ready);
}

RecordingThread::~RecordingThread()
{
    //録音スレッド停止
    m_recordThread.exit();
    m_recordThread.wait();
}
//録画開始
void RecordingThread::startRecording()
{
    qDebug() << "start recording timer " << m_et.elapsed();

    //状態変更
    setStatus(Recording);

    //クリア
    clearCaptureFiles();
    m_recordingCounter = 0;
    m_SaveDataList.clear();
    m_et.start();

    //録音開始
    QString audio_path = getTempAudioPath();
    emit audioRecord(QUrl::fromLocalFile(audio_path), audioInputName());

    //タイマー開始
    m_stop = false;
    m_timer->start(static_cast<int>(1000.0/fps()));
}
//録画終了
void RecordingThread::stopRecording()
{
    //状態変更
    setStatus(Convert);

    //録音停止
    m_audio.stop();
    //タイマー停止
    m_timer->stop();
    m_stop = true;

    qDebug() << "stop recording timer:" << m_recordingCounter;

    //終了を待つ
    if(isRunning()) wait();

    //残骸チェック
    foreach (SaveData data, m_SaveDataList) {
        qDebug() << "remnant:" << data.image.isNull() << "," << data.path;
    }

    //変換
    convert();

    //デバッグ計測
    qint64 pt = 0;
    foreach (qint64 t, m_interval) {
        qDebug() << t << "(" << (t-pt) << ")";
        pt = t;
    }
}
//保存フォルダ（テンポラリの中身をクリア）
void RecordingThread::clearCaptureFiles()
{
    QDir dir(getTempPath());
    QFileInfoList list = dir.entryInfoList();
    //ファイルを消す
    for(int i=0; i<list.length(); i++){
        QFile::remove(list[i].filePath());
    }
}

WebView *RecordingThread::webView() const
{
    return m_webView;
}

void RecordingThread::setWebView(WebView *webview)
{
    m_webView = webview;
}
//保存パス
QString RecordingThread::savePath() const
{
    return m_savePath;
}
void RecordingThread::setSavePath(const QString &savePath)
{
    m_savePath = savePath;
}
//テンポラリパス（強制変更）
QString RecordingThread::tempPath() const
{
    return m_tempPath;
}
void RecordingThread::setTempPath(const QString &tempPath)
{
    m_tempPath = tempPath;
}
//ツールのパス
QString RecordingThread::toolPath() const
{
    return m_toolPath;
}
void RecordingThread::setToolPath(const QString &toolPath)
{
    m_toolPath = toolPath;
}
//ツールのパラメータ
QString RecordingThread::toolParam() const
{
    return m_toolParam;
}
void RecordingThread::setToolParam(const QString &toolParam)
{
    m_toolParam = toolParam;
}

//プロセス終了
void RecordingThread::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "RecordingThread::processFinished , exitCode=" << exitCode << ", exitStatus=" << exitStatus;

    QByteArray output = m_process.readAllStandardError();
    QString str = QString::fromLocal8Bit( output );
    qDebug() << ">" << str;

    //状態変更
    setStatus(Ready);
}
//プロセスエラー
void RecordingThread::processError(QProcess::ProcessError error)
{
    qDebug() << "RecordingThread::processError " << error;

    //状態変更
    setStatus(Ready);
}
//録音エラー
void RecordingThread::audioRecordingError(QMediaRecorder::Error error)
{
    qDebug() << "Recording Audio Error:" << error;
}
RecordingThread::RecordingStatus RecordingThread::status() const
{
    return m_status;
}
void RecordingThread::setStatus(const RecordingStatus &status)
{
    if(m_status == status)  return;
    m_status = status;
    emit statusChanged(status);
}

//オーディオ録音するデバイス名
QString RecordingThread::audioInputName() const
{
    if(m_audioInputName.isEmpty()){
        return m_audio.defaultAudioInput();
    }else{
        return m_audioInputName;
    }
}
void RecordingThread::setAudioInputName(const QString &audioInputName)
{
    m_audioInputName = audioInputName;
}
//オーディオデバイスの一覧
QStringList RecordingThread::audioInputNames() const
{
    return m_audio.audioInputs();
}

//フレームレート
qreal RecordingThread::fps() const
{
    return m_fps;
}
void RecordingThread::setFps(const qreal &fps)
{
    m_fps = fps;
}







//キャプチャしてデータをリストへ突っ込む
void RecordingThread::capture(unsigned long count)
{
    QImage img = webView()->capture();
    if(img.isNull()){
        qDebug() << "don't capture";
        return;
    }
    qint64 elapsed = m_et.elapsed();
    qint64 duration = m_audio.duration();
    SaveData data(img
                  , QString("%1/kanmemo_%2.jpg").arg(getTempPath()).arg(count, 6, 10, QChar('0'))
                  , elapsed, duration);

    m_mutex.lock();
    m_SaveDataList.append(data);
    m_mutex.unlock();
}
//静止画を動画に変換する
void RecordingThread::convert()
{
    QString image_path = QString("%1/kanmemo_%6d.jpg").arg(getTempPath());
    QString audio_path = getTempAudioPath();
#if defined(Q_OS_WIN)
    image_path = image_path.replace(QStringLiteral("/"), QStringLiteral("\\"));
#endif

    QFileInfo fi(audio_path);

    QStringList args;
    args.append("-r");
    args.append(QString::number(fps()));
    args.append("-i");
    args.append(image_path);
    if(fi.exists()){
        args.append("-i");
        args.append(audio_path);
    }
    args.append("-vcodec");
    args.append("libx264");
    args.append("-qscale:v");
    args.append("0");
    args.append("-y");
    args.append(savePath());

    QString args_str;
    foreach (QString arg, args) {
        args_str += arg + " ";
    }
    qDebug() << args_str;

    if(m_process.state() == QProcess::NotRunning){
        m_process.start(toolPath(), args);
    }else{
        qDebug() << "not start convert process.(ffmpeg)";
    }
}
//最新のカウンタを取得
unsigned long RecordingThread::getRecordingCounter()
{
    return m_recordingCounter++;
}
//連続データの保存先の取得
QString RecordingThread::getTempPath()
{
    QString path;
    path = tempPath();
    //なければつくる
    QDir dir(path);
    if(!dir.exists())
        dir.mkpath(path);

    return path;
}
//録音ファイルのパスを取得
QString RecordingThread::getTempAudioPath()
{
    QString audio_path = QString("%1/kanmemo.wav").arg(getTempPath());
#if defined(Q_OS_WIN)
    audio_path = audio_path.replace(QStringLiteral("/"), QStringLiteral("\\"));
#endif
    return audio_path;
}




void RecordingThread::run()
{
    qDebug() << "start recoding thread " << QString::number(m_et.elapsed());

    //録画
    bool empty;
    qreal frame_time = 1000.0 / fps();
    qint64 elapse;

    unsigned long count = 0;
    //デバッグ
    unsigned long timer_time = 0;
    qint64 start_offset_time = m_SaveDataList.first().elapse;
    qDebug() << "start_offset_time:" << start_offset_time;

    m_mutex.lock();
    empty = m_SaveDataList.isEmpty();
    m_mutex.unlock();
    while(!empty){
        //保存
        SaveData data = m_SaveDataList.first();
        save(data, count++);

        timer_time = (count-1)*1000/fps();
        elapse = data.elapse - start_offset_time;
//        qDebug() << count << " , " << timer_time << "-" << elapse << "=" << (timer_time - elapse)
//                 << " : " << data.duration << "-" << elapse << "=" << (data.duration - elapse);

        //次があるか確認
        m_mutex.lock();
        m_SaveDataList.removeFirst();
        empty = m_SaveDataList.isEmpty();
        m_mutex.unlock();

        //停止指示がかかってない時は待つ
        while(empty && m_stop == false){
//                qDebug() << "wait empty " << m_stop << "," << data.path;
            msleep(static_cast<unsigned long>(2000.0/fps()));
            empty = m_SaveDataList.isEmpty();
        }

        //設定フレームあったか確認
        if(!empty){
            //保存しようとしているフレームの理論時間
            timer_time = (count-1)*1000/fps();
            elapse = data.elapse - start_offset_time;
            //理論時間とのズレが1フレームより大きい間補間する
            while(abs(timer_time - elapse) > frame_time){
                //保存
                save(data, count++);

                //保存しようとしているフレームの理論時間
                timer_time = (count-1)*1000/fps();
                elapse = data.elapse - start_offset_time;

                //保存した時間
//                qDebug() << count << " , " << timer_time << "-" << elapse << "=" << (timer_time - elapse)
//                         << " : " << data.duration << "-" << elapse << "=" << (data.duration - elapse)
//                         << " *";
            }
        }
    }

    qDebug() << "stop recoding thread " << QString::number(m_et.elapsed());
}

void RecordingThread::save(SaveData &data, unsigned long count)
{
    QString path = QString("%1/kanmemo_%2.jpg").arg(getTempPath()).arg(count, 6, 10, QChar('0'));
    if(data.image.isNull()){
        qDebug() << "list data is null";
    }else if(data.image.save(path, "jpg")){
        //ok
    }else{
        //ng
        qDebug() << "failed save";
    }

}
