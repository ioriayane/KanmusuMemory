#ifndef INPUTOAUTHPINDIALOG_H
#define INPUTOAUTHPINDIALOG_H

#include <QDialog>

namespace Ui {
class InputOAuthPinDialog;
}

class InputOAuthPinDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit InputOAuthPinDialog(QWidget *parent = 0);
    ~InputOAuthPinDialog();
    
    QString pin() const;
    void setPin(const QString &pin);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::InputOAuthPinDialog *ui;

    QString m_pin;
};

#endif // INPUTOAUTHPINDIALOG_H
