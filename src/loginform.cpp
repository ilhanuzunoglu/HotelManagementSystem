// loginform.cpp 
#include "loginform.h"
#include "ui_loginform.h"
#include "database.h"
#include "mainwindow.h"
#include "userdashboard.h"
#include "receptionistdashboard.h"
#include "admindashboard.h"
#include <QMessageBox>

LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::LoginForm), mainWindow(nullptr)
{
    ui->setupUi(this);
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginForm::on_loginButton_clicked);
    connect(ui->registerLinkButton, &QPushButton::clicked, this, &LoginForm::on_registerLinkButton_clicked);
    connect(ui->backButton, &QPushButton::clicked, this, &LoginForm::on_backButton_clicked);
}

LoginForm::~LoginForm() { delete ui; }

void LoginForm::setMainWindow(MainWindow* mw) { mainWindow = mw; }

void LoginForm::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    User user = Database::instance().authenticateUser(username, password);
    if (!user.username.isEmpty()) {
        emit loginSuccess(user);
        ui->errorLabel->clear();
        this->hide();
    } else {
        ui->errorLabel->setText("Kullanıcı adı veya şifre hatalı!");
    }
}

void LoginForm::on_registerLinkButton_clicked()
{
    emit registerLinkClicked();
}

void LoginForm::on_backButton_clicked()
{
    emit backClicked();
} 