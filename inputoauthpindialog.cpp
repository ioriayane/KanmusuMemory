/*
 * Copyright 2013 KanMemo Project.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
