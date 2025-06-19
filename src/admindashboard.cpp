// admindashboard.cpp 
#include "admindashboard.h"
#include "ui_admindashboard.h"
#include "mainwindow.h"

AdminDashboard::AdminDashboard(QWidget *parent)
    : QWidget(parent), ui(new Ui::AdminDashboard), mainWindow(nullptr)
{
    ui->setupUi(this);
    connect(ui->logoutButton, &QPushButton::clicked, this, &AdminDashboard::on_logoutButton_clicked);
}

AdminDashboard::~AdminDashboard() { delete ui; }

void AdminDashboard::setUser(const User& user)
{
    currentUser = user;
    ui->welcomeLabel->setText("Hoş Geldiniz, " + user.username);
}

void AdminDashboard::on_logoutButton_clicked()
{
    emit logoutClicked();
} 