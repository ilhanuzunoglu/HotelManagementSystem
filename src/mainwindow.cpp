#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginform.h"
#include "registerform.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), loginForm(nullptr), registerForm(nullptr)
{
    ui->setupUi(this);
    if (ui->loginButton)
        connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::on_loginButton_clicked);
    if (ui->registerButton)
        connect(ui->registerButton, &QPushButton::clicked, this, &MainWindow::on_registerButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loginButton_clicked()
{
    if (!loginForm) loginForm = new LoginForm(this);
    loginForm->show();
    this->hide();
}

void MainWindow::on_registerButton_clicked()
{
    if (!registerForm) registerForm = new RegisterForm(this);
    registerForm->show();
    this->hide();
} 