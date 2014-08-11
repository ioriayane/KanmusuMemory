#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QAudioRecorder>

class AudioRecorder : public QAudioRecorder
{
    Q_OBJECT

public:
    AudioRecorder(QObject* parent=0);

public slots:
    void record(const QUrl& outPath, const QString& audioInput);
};

#endif // AUDIORECORDER_H
