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

void ReceptionistDashboard::on_logoutButton_clicked()
{
    if (!mainWindow) mainWindow = qobject_cast<MainWindow*>(parentWidget());
    if (mainWindow) {
        mainWindow->show();
        this->hide();
    }
} 