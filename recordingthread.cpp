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
  , m_audio(this)
  , m_stop(true)
{
    m_et.start();

    //録音
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/mpeg");
    audioSettings.setQuality(QMultimedia::HighQuality);
    m_audio.setEncodingSettings(audioSettings);

    //タイマー
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [this](){
        static bool f = true;
        if(f){
            qDebug() << "timer first " << m_et.elapsed();
            f = false;
        }

        //デバッグ計測
//        m_interval.append(m_et.elapsed());
        //キャプチャ
        unsigned long counter = getRecordingCounter();
//        qDebug() << "trigger timer " << counter;
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

//    //プロセスの状態が変わった
//    connect(&m_process, &QProcess::stateChanged, [this](QProcess::ProcessState newState){
//        qDebug() << "QProcess::stateChanged " << newState;
//        switch(newState){
//        case QProcess::NotRunning:
//            break;
//        case QProcess::Starting:
//            break;
//        case QProcess::Running:
//            //動き出した
//            break;
//        default:
//            break;
//        }
//    });

    //プロセスが終了した
    connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)) );
    //プロセスがエラー
    connect(&m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)) );
    //録音がエラー
    connect(&m_audio, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(audioRecordingError(QMediaRecorder::Error)));
//    connect(&m_audio, &QAudioRecorder::stateChanged, [this](QMediaRecorder::State state){
//        qint64 t = m_et.elapsed();
//        qDebug() << "QAudioRecorder::stateChanged " << state << "," << QString::number(t);
//    });
//    connect(&m_audio, &QAudioRecorder::statusChanged, [this](QMediaRecorder::Status status){
//        qint64 t = m_et.elapsed();
//        qDebug() << "QAudioRecorder::statusChanged " << status << "," << QString::number(t);
//    });
}
//録画開始
void RecordingThread::startRecording()
{
    qDebug() << "start recording timer " << m_et.elapsed();

    //クリア
    clearCaptureFiles();
    m_recordingCounter = 0;
    m_SaveDataList.clear();
    m_et.start();

    //録音開始
    QString audio_path = QString("%1/kanmemo.mp3").arg(getTempPath());
#if defined(Q_OS_WIN)
    audio_path = audio_path.replace(QStringLiteral("/"), QStringLiteral("\\"));
#endif
    m_audio.setOutputLocation(QUrl::fromLocalFile(audio_path));
    m_audio.setAudioInput(audioInputName());
    qDebug() << "audio out:" << audio_path << "," << m_audio.outputLocation();
    qDebug() << "audio input:" << m_audio.audioInput();
    m_audio.record();

    //タイマー開始
    m_stop = false;
    m_state = Recording;
    m_timer->start(static_cast<int>(1000.0/fps()));
}
//録画終了
void RecordingThread::stopRecording()
{
    //録音停止
    m_audio.stop();
    //タイマー停止
    m_timer->stop();
    m_stop = true;

    qDebug() << "stop recording timer:" << m_recordingCounter;

    //終了を待つ
    m_state = Converting;
    if(isRunning()) wait();

    //残骸チェック
    foreach (SaveData data, m_SaveDataList) {
        qDebug() << "remnant:" << data.image.isNull() << "," << data.path;
    }

    //変換
    if(m_state == Converting){
        //画像を変換する
        convert();

        m_state = Stop;
    }

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

    QByteArray output = m_process.readAllStandardOutput();
    QString str = QString::fromLocal8Bit( output );
    qDebug() << ">" << str;
}
//プロセスエラー
void RecordingThread::processError(QProcess::ProcessError error)
{
    qDebug() << "RecordingThread::processError " << error;

}
//録音エラー
void RecordingThread::audioRecordingError(QMediaRecorder::Error error)
{
    qDebug() << "Recording Audio Error:" << error;
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
    SaveData data(img
                  , QString("%1/kanmemo_%2.jpg").arg(getTempPath()).arg(count, 6, 10, QChar('0'))
                  , m_et.elapsed());

    m_mutex.lock();
    m_SaveDataList.append(data);
    m_mutex.unlock();
}
//静止画を動画に変換する
void RecordingThread::convert()
{
    QString image_path = QString("%1/kanmemo_%6d.jpg").arg(getTempPath());
    QString audio_path = QString("%1/kanmemo.mp3").arg(getTempPath());
#if defined(Q_OS_WIN)
    image_path = image_path.replace(QStringLiteral("/"), QStringLiteral("\\"));
    audio_path = audio_path.replace(QStringLiteral("/"), QStringLiteral("\\"));
#endif

//#error フレームレートは計測しないとダメかも
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

    m_process.start(toolPath(), args);

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

    if(tempPath().isEmpty()){
        path = QString("%1/%2/%3")
                .arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                .arg(KANMEMO_PROJECT)
                .arg("recording");
    }else{
        path = tempPath();
    }
    //なければつくる
    QDir dir(path);
    if(!dir.exists())
        dir.mkpath(path);

    return path;
}




void RecordingThread::run()
{
    qDebug() << "start recoding thread " << QString::number(m_et.elapsed());


    if(m_state == Recording){
        bool empty;
        qint64 interval = 1000/4;
        qint64 next_frame = interval;
        int frame_per_sec = static_cast<int>(fps()/4.0);
        int frame_count = 0;//static_cast<int>(fps());
        unsigned long count = 0;

        m_mutex.lock();
        empty = m_SaveDataList.isEmpty();
        m_mutex.unlock();
//        if(!empty){
//            next_frame = m_SaveDataList.at(0).elapse + interval;
//        }
        while(!empty){
            //保存
            SaveData data = m_SaveDataList.first();
            save(data, count++);

            //フレーム数
            frame_count++;

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
                if(m_SaveDataList.first().elapse > next_frame){
                    //次のキャプチャは今回の1秒に入ってない
                    if(frame_count < frame_per_sec){
                        //基底のフレーム数に達してない
                        qDebug() << "shortness frame " << data.elapse << "," << frame_count << "-" << frame_per_sec << "," << (frame_per_sec - frame_count);
                        for(int fc=frame_count; fc < frame_per_sec; fc++){
                            //不足分を補充
                            save(data, count++);
                        }
                    }
                    //フレーム数クリア
                    frame_count = 0;
                    //次の判定時間
                    next_frame += interval;
                }
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
    qDebug() << count << "," << data.elapse;
}
