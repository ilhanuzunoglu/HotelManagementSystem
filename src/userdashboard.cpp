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

void UserDashboard::on_logoutButton_clicked()
{
    if (!mainWindow) mainWindow = qobject_cast<MainWindow*>(parentWidget());
    if (mainWindow) {
        mainWindow->show();
        this->hide();
    }
} 