#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    
    const QString &savePath() const;
    void setSavePath(const QString &save_path);

    static QString selectSavePath(const QString &current_path);

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_selectPathButton_clicked();

private:
    Ui::SettingsDialog *ui;

    QString m_savePath;
};

#endif // SETTINGSDIALOG_H
