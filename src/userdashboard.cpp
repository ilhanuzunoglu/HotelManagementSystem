// userdashboard.cpp 
#include "userdashboard.h"
#include "ui_userdashboard.h"
#include "mainwindow.h"

UserDashboard::UserDashboard(QWidget *parent)
    : QWidget(parent), ui(new Ui::UserDashboard), mainWindow(nullptr)
{
    ui->setupUi(this);
    connect(ui->logoutButton, &QPushButton::clicked, this, &UserDashboard::on_logoutButton_clicked);
}

UserDashboard::~UserDashboard() { delete ui; }

void UserDashboard::setUser(const User& user)
{
    currentUser = user;
    ui->welcomeLabel->setText("Hoş Geldiniz, " + user.username);
}

void UserDashboard::on_logoutButton_clicked()
{
    emit logoutClicked();
} 