#include "recordingdialog.h"
#include "ui_recordingdialog.h"

#include <QFile>
#include <QMessageBox>
#include <QAudioRecorder>
#include <QFileDialog>
#include <QToolButton>


class RecordingDialog::Private
{
public:
    Private(RecordingDialog *parent);

private:
    RecordingDialog *q;
    Ui::RecordingDialog ui;

public:
    int fps;
    QString audioSource;
    QString savePath;
    QString toolPath;
    QString tempPath;
    int soundOffset;
    bool dontViewButtleResult;
    bool muteNotificationSound;
};


RecordingDialog::Private::Private(RecordingDialog *parent)
    : q(parent)
{
    ui.setupUi(q);

    connect(ui.buttonBox, &QDialogButtonBox::accepted, [this](){
        if(!QFile::exists(ui.savePathEdit->text())){
            QMessageBox::warning(q, "warning", "save path not found.");
            return;
        }
        if(!QFile::exists(ui.toolPathEdit->text())){
            QMessageBox::warning(q, "warning", "Tool not found.");
            return;
        }
        if(!QFile::exists(ui.tempPathEdit->text())){
            QMessageBox::warning(q, "warning", "Temp path not found.");
            return;
        }

        fps = ui.fpsSpinBox->value();
        audioSource = ui.audioSourceComboBox->currentText();
        savePath = ui.savePathEdit->text();
        toolPath = ui.toolPathEdit->text();
        tempPath = ui.tempPathEdit->text();
        soundOffset = ui.soundOffsetSpinBox->value();
        dontViewButtleResult = ui.dontViewButtleResultCheckBox->isChecked();
        muteNotificationSound = ui.muteNotificationSoundCheckBox->isChecked();

        q->accept();
    });
    connect(ui.buttonBox, &QDialogButtonBox::rejected, q, &QDialog::reject);

    QAudioRecorder recorder;
    ui.audioSourceComboBox->insertItems(0, recorder.audioInputs());

    connect(q, &RecordingDialog::fpsChanged, ui.fpsSpinBox, &QSpinBox::setValue);
    connect(q, &RecordingDialog::audioSourceChanged, ui.audioSourceComboBox, &QComboBox::setCurrentText);
    connect(q, &RecordingDialog::savePathChanged, ui.savePathEdit, &QLineEdit::setText);
    connect(q, &RecordingDialog::toolPathChanged, ui.toolPathEdit, &QLineEdit::setText);
    connect(q, &RecordingDialog::tempPathChanged, ui.tempPathEdit, &QLineEdit::setText);
    connect(q, &RecordingDialog::soundOffsetChanged, ui.soundOffsetSpinBox, &QSpinBox::setValue);
    connect(q, &RecordingDialog::dontViewButtleResultChanged, ui.dontViewButtleResultCheckBox, &QCheckBox::setChecked);
    connect(q, &RecordingDialog::muteNotificationSoundChanged, ui.muteNotificationSoundCheckBox, &QCheckBox::setChecked);

    //保存パスの参照ボタン
    connect(ui.selectSavePathButton, &QToolButton::clicked, [this]() {
        QString defaultpath = ui.savePathEdit->text();
        if(!QFile::exists(defaultpath)){
            defaultpath = q->defaultSavePath();
        }
        QString path = QFileDialog::getExistingDirectory(q, tr("Select save folder"),
                                                         defaultpath,
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);
        if(QFile::exists(path)){
            q->setSavePath(path);
        }
    });
    //ツールパス
    connect(ui.selectToolPathButton, &QToolButton::clicked, [this]() {
        QString defaultpath = ui.toolPathEdit->text();
        if(!QFile::exists(defaultpath)){
            defaultpath = q->defaultToolPath();
        }
        QString path = QFileDialog::getOpenFileName(q, tr("Select ffmpeg"),
                                                    defaultpath,
                                                    "Execute(*.exe)");
        if(QFile::exists(path)){
            q->setToolPath(path);
        }
    });
    //テンポラリパス
    connect(ui.selectTempPathButton, &QToolButton::clicked, [this]() {
        QString defaultpath = ui.tempPathEdit->text();
        if(!QFile::exists(defaultpath)){
            defaultpath = q->defaultTempPath();
        }
        QString path = QFileDialog::getExistingDirectory(q, tr("Temp save folder"),
                                                         defaultpath,
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);
        if(QFile::exists(path)){
            q->setTempPath(path);
        }
    });

    //デフォルトボタン
    connect(ui.defaultButton, &QPushButton::clicked, [this](bool checked){
        Q_UNUSED(checked);
        ui.fpsSpinBox->setValue(q->defaultFps());
        ui.audioSourceComboBox->setCurrentText(q->defaultAudioSource());
        ui.savePathEdit->setText(q->defaultSavePath());
        ui.toolPathEdit->setText(q->defaultToolPath());
        ui.tempPathEdit->setText(q->defaultTempPath());
        ui.soundOffsetSpinBox->setValue(q->defaultSoundOffset());
        ui.dontViewButtleResultCheckBox->setChecked(q->defaultDontViewButtleResult());
        ui.muteNotificationSoundCheckBox->setChecked(q->defaultMuteNotificationSound());
    });
}



RecordingDialog::RecordingDialog(QWidget *parent) :
    QDialog(parent),
    d(new Private(this))
{
    connect(this, &QObject::destroyed, [this]() { delete d; });
}

RecordingDialog::~RecordingDialog()
{
}

int RecordingDialog::fps() const
{
    return d->fps;
}
void RecordingDialog::setFps(int value)
{
    if(d->fps == value) return;
    d->fps = value;
    emit fpsChanged(value);
}

QString RecordingDialog::audioSource() const
{
    return d->audioSource;
}
void RecordingDialog::setAudioSource(const QString &value)
{
    if(d->audioSource.compare(value) == 0)  return;
    d->audioSource = value;
    emit audioSourceChanged(value);
}

QString RecordingDialog::savePath() const
{
    return d->savePath;
}
void RecordingDialog::setSavePath(const QString &value)
{
    if(d->savePath.compare(value) == 0) return;
    d->savePath = value;
    emit savePathChanged(value);
}

QString RecordingDialog::toolPath() const
{
    return d->toolPath;
}
void RecordingDialog::setToolPath(const QString &value)
{
    if(d->toolPath.compare(value) == 0) return;
    d->toolPath = value;
    emit toolPathChanged(value);
}

QString RecordingDialog::tempPath() const
{
    return d->tempPath;
}

int RecordingDialog::soundOffset() const
{
    return d->soundOffset;
}
void RecordingDialog::setTempPath(const QString &value)
{
    if(d->tempPath.compare(value) == 0) return;
    d->tempPath = value;
    emit tempPathChanged(value);
}

void RecordingDialog::setSoundOffset(int offset)
{
    if(d->soundOffset == offset)    return;
    d->soundOffset = offset;
    emit soundOffsetChanged(offset);
}

bool RecordingDialog::dontViewButtleResult() const
{
    return d->dontViewButtleResult;
}

bool RecordingDialog::muteNotificationSound() const
{
    return d->muteNotificationSound;
}
void RecordingDialog::setDontViewButtleResult(bool dontViewButtleResult)
{
    if(d->dontViewButtleResult == dontViewButtleResult) return;
    d->dontViewButtleResult = dontViewButtleResult;
    emit dontViewButtleResultChanged(dontViewButtleResult);
}

void RecordingDialog::setMuteNotificationSound(bool mute)
{
    if(d->muteNotificationSound == mute)    return;
    d->muteNotificationSound = mute;
    emit muteNotificationSoundChanged(mute);
}
int RecordingDialog::defaultSoundOffset() const
{
    return m_defaultSoundOffset;
}
void RecordingDialog::setDefaultSoundOffset(int defaultSoundOffset)
{
    m_defaultSoundOffset = defaultSoundOffset;
}

bool RecordingDialog::defaultMuteNotificationSound() const
{
    return m_defaultMuteNotificationSound;
}
void RecordingDialog::setDefaultMuteNotificationSound(bool defaultMuteNotificationSound)
{
    m_defaultMuteNotificationSound = defaultMuteNotificationSound;
}


bool RecordingDialog::defaultDontViewButtleResult() const
{
    return m_defaultDontViewButtleResult;
}
void RecordingDialog::setDefaultDontViewButtleResult(bool defaultDontViewButtleResult)
{
    m_defaultDontViewButtleResult = defaultDontViewButtleResult;
}


QString RecordingDialog::defaultAudioSource() const
{
    return m_defaultAudioSource;
}

void RecordingDialog::setDefaultAudioSource(const QString &defaultAudioSource)
{
    m_defaultAudioSource = defaultAudioSource;
}

int RecordingDialog::defaultFps() const
{
    return m_defaultFps;
}

void RecordingDialog::setDefaultFps(int defaultFps)
{
    m_defaultFps = defaultFps;
}


QString RecordingDialog::defaultSavePath() const
{
    return m_defaultSavePath;
}

void RecordingDialog::setDefaultSavePath(const QString &defaultSavePath)
{
    m_defaultSavePath = defaultSavePath;
}

QString RecordingDialog::defaultToolPath() const
{
    return m_defaultToolPath;
}

void RecordingDialog::setDefaultToolPath(const QString &defaultToolPath)
{
    m_defaultToolPath = defaultToolPath;
}

QString RecordingDialog::defaultTempPath() const
{
    return m_defaultTempPath;
}

void RecordingDialog::setDefaultTempPath(const QString &defaultTempPath)
{
    m_defaultTempPath = defaultTempPath;
}







