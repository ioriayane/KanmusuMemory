#include "inputoauthpindialog.h"
#include "ui_inputoauthpindialog.h"

InputOAuthPinDialog::InputOAuthPinDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputOAuthPinDialog)
{
    ui->setupUi(this);
}

InputOAuthPinDialog::~InputOAuthPinDialog()
{
    delete ui;
}

QString InputOAuthPinDialog::pin() const
{
    return m_pin;
}

void InputOAuthPinDialog::setPin(const QString &pin)
{
    m_pin = pin;
}

//OK押したらPINを保存して終了
void InputOAuthPinDialog::on_buttonBox_accepted()
{
    setPin(ui->lineEdit->text());
}
