#include "audiorecorder.h"

AudioRecorder::AudioRecorder(QObject *parent)
    : QAudioRecorder(parent)
{
}

void AudioRecorder::record(const QUrl &outPath, const QString &audioInput)
{
    setOutputLocation(outPath);
    setAudioInput(audioInput);
    QAudioRecorder::record();
}

