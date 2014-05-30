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

private:
    Ui::RecordingDialog *ui;
};

#endif // RECORDINGDIALOG_H
