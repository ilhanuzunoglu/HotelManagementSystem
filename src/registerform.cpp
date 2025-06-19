// registerform.cpp 
#include "registerform.h"
#include "ui_registerform.h"
#include "database.h"
#include "mainwindow.h"
#include "loginform.h"

RegisterForm::RegisterForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::RegisterForm), mainWindow(nullptr), loginForm(nullptr)
{
    ui->setupUi(this);
    connect(ui->registerButton, &QPushButton::clicked, this, &RegisterForm::on_registerButton_clicked);
    connect(ui->backButton, &QPushButton::clicked, this, &RegisterForm::on_backButton_clicked);
}

RegisterForm::~RegisterForm() { delete ui; }

void RegisterForm::on_registerButton_clicked()
{
    QString name = ui->nameLineEdit->text().trimmed();
    QString surname = ui->surnameLineEdit->text().trimmed();
    QString username = ui->usernameLineEdit->text().trimmed();
    QString email = ui->emailLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString passwordRepeat = ui->passwordRepeatLineEdit->text();
    QString errorMsg;
    if (name.isEmpty() || surname.isEmpty() || username.isEmpty() || email.isEmpty() || password.isEmpty() || passwordRepeat.isEmpty()) {
        ui->errorLabel->setText("Tüm alanları doldurun.");
        return;
    }
    if (password != passwordRepeat) {
        ui->errorLabel->setText("Şifreler uyuşmuyor.");
        return;
    }
    if (Database::instance().registerUser(name, surname, username, email, password, errorMsg)) {
        ui->errorLabel->setStyleSheet("color: #27AE60;");
        ui->errorLabel->setText("Kayıt başarılı! Giriş yapabilirsiniz.");
        if (!loginForm) loginForm = new LoginForm(this);
        loginForm->show();
        this->hide();
    } else {
        ui->errorLabel->setStyleSheet("color: #E74C3C;");
        ui->errorLabel->setText(errorMsg);
    }
}

void RegisterForm::on_backButton_clicked()
{
    if (!mainWindow) mainWindow = qobject_cast<MainWindow*>(parentWidget());
    if (mainWindow) {
        mainWindow->show();
        this->hide();
    }
} 