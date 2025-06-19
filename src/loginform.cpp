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
    : QWidget(parent), ui(new Ui::LoginForm), mainWindow(nullptr), userDashboard(nullptr), receptionistDashboard(nullptr), adminDashboard(nullptr)
{
    ui->setupUi(this);
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginForm::on_loginButton_clicked);
    connect(ui->registerLinkButton, &QPushButton::clicked, this, &LoginForm::on_registerLinkButton_clicked);
    connect(ui->backButton, &QPushButton::clicked, this, &LoginForm::on_backButton_clicked);
}

LoginForm::~LoginForm() { delete ui; }

void LoginForm::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString role;
    if (Database::instance().authenticateUser(username, password, role)) {
        if (role == "user") {
            if (!userDashboard) userDashboard = new UserDashboard(this);
            userDashboard->show();
        } else if (role == "receptionist") {
            if (!receptionistDashboard) receptionistDashboard = new ReceptionistDashboard(this);
            receptionistDashboard->show();
        } else if (role == "admin") {
            if (!adminDashboard) adminDashboard = new AdminDashboard(this);
            adminDashboard->show();
        }
        this->hide();
        ui->errorLabel->clear();
    } else {
        ui->errorLabel->setText("Kullanıcı adı veya şifre hatalı!");
    }
}

void LoginForm::on_registerLinkButton_clicked()
{
    if (!mainWindow) mainWindow = qobject_cast<MainWindow*>(parentWidget());
    if (mainWindow) {
        mainWindow->on_registerButton_clicked();
        this->hide();
    }
}

void LoginForm::on_backButton_clicked()
{
    if (!mainWindow) mainWindow = qobject_cast<MainWindow*>(parentWidget());
    if (mainWindow) {
        mainWindow->show();
        this->hide();
    }
} 