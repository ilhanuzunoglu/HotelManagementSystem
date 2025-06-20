// registerform.cpp 
#include "registerform.h"
#include "ui_registerform.h"
#include "database.h"
#include "mainwindow.h"
#include "loginform.h"
#include <QRegularExpressionValidator>

RegisterForm::RegisterForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::RegisterForm), mainWindow(nullptr)
{
    ui->setupUi(this);
    phoneLineEdit = new QLineEdit(this);
    phoneLineEdit->setPlaceholderText("(5xx) xxx xx xx");
    phoneLineEdit->setStyleSheet("");
    QRegularExpression phoneRegex(R"(^5[0-9]{9}$)");
    phoneLineEdit->setValidator(new QRegularExpressionValidator(phoneRegex, this));
    ui->frameLayout->insertWidget(4, phoneLineEdit);
    ui->emailLineEdit->setPlaceholderText("ornek@mail.com");
    ui->emailLineEdit->setStyleSheet("");
    connect(ui->registerButton, &QPushButton::clicked, this, &RegisterForm::on_registerButton_clicked);
    connect(ui->backButton, &QPushButton::clicked, this, &RegisterForm::on_backButton_clicked);
}

RegisterForm::~RegisterForm() { delete ui; }

void RegisterForm::setMainWindow(MainWindow* mw) { mainWindow = mw; }

void RegisterForm::on_registerButton_clicked()
{
    QString name = ui->nameLineEdit->text().trimmed();
    QString surname = ui->surnameLineEdit->text().trimmed();
    QString username = ui->usernameLineEdit->text().trimmed();
    QString email = ui->emailLineEdit->text().trimmed();
    QString phone = phoneLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString passwordRepeat = ui->passwordRepeatLineEdit->text();
    QString errorMsg;
    QRegularExpression emailRegex(R"(^[\w\.-]+@[\w\.-]+\.[a-zA-Z]{2,}$)");
    if (name.isEmpty() || surname.isEmpty() || username.isEmpty() || email.isEmpty() || phone.isEmpty() || password.isEmpty() || passwordRepeat.isEmpty()) {
        ui->errorLabel->setText("Tüm alanları doldurun.");
        return;
    }
    if (!emailRegex.match(email).hasMatch()) {
        ui->errorLabel->setText("Geçerli bir e-posta girin.");
        return;
    }
    if (!phoneLineEdit->hasAcceptableInput()) {
        ui->errorLabel->setText("Geçerli bir telefon numarası girin (10-15 rakam).");
        return;
    }
    if (password != passwordRepeat) {
        ui->errorLabel->setText("Şifreler uyuşmuyor.");
        return;
    }
    if (Database::instance().registerUser(name, surname, username, email, phone, password, errorMsg)) {
        User user = Database::instance().getUser(username);
        emit registerSuccess(user);
        ui->errorLabel->setStyleSheet("color: #27AE60;");
        ui->errorLabel->setText("Kayıt başarılı! Giriş yapabilirsiniz.");
    } else {
        ui->errorLabel->setStyleSheet("color: #E74C3C;");
        ui->errorLabel->setText(errorMsg);
    }
}

void RegisterForm::on_backButton_clicked()
{
    emit backClicked();
} 