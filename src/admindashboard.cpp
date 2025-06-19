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

void AdminDashboard::on_logoutButton_clicked()
{
    if (!mainWindow) mainWindow = qobject_cast<MainWindow*>(parentWidget());
    if (mainWindow) {
        mainWindow->show();
        this->hide();
    }
} 