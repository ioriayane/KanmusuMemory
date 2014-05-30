#include "recordingdialog.h"
#include "ui_recordingdialog.h"

RecordingDialog::RecordingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecordingDialog)
{
    ui->setupUi(this);
}

RecordingDialog::~RecordingDialog()
{
    delete ui;
}
