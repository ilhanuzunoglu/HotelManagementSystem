// receptionistdashboard.cpp 
#include "receptionistdashboard.h"
#include "ui_receptionistdashboard.h"
#include "mainwindow.h"

ReceptionistDashboard::ReceptionistDashboard(QWidget *parent)
    : QWidget(parent), ui(new Ui::ReceptionistDashboard), mainWindow(nullptr)
{
    ui->setupUi(this);
    connect(ui->logoutButton, &QPushButton::clicked, this, &ReceptionistDashboard::on_logoutButton_clicked);
}

ReceptionistDashboard::~ReceptionistDashboard() { delete ui; }

void ReceptionistDashboard::setUser(const User& user)
{
    currentUser = user;
    ui->welcomeLabel->setText("Hoş Geldiniz, " + user.username);
}

void ReceptionistDashboard::on_logoutButton_clicked()
{
    emit logoutClicked();
} 