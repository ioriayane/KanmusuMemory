#include "audiorecorder.h"
#include <QDebug>
#include <QUrl>

AudioRecorder::AudioRecorder(QObject *parent)
    : QAudioRecorder(parent)
{
}

void AudioRecorder::record(const QUrl &outPath, const QString &audioInput)
{
    setOutputLocation(outPath);
    setAudioInput(audioInput);
    QAudioRecorder::record();

    qDebug() << "audio out:" << outputLocation().toString();
    qDebug() << "audio input:" << QAudioRecorder::audioInput();
}

