#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginform.h"
#include "registerform.h"
#include "userdashboard.h"
#include "receptionistdashboard.h"
#include "admindashboard.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      loginForm(nullptr), registerForm(nullptr),
      userDashboard(nullptr), receptionistDashboard(nullptr), adminDashboard(nullptr)
{
    ui->setupUi(this);
    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::on_loginButton_clicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &MainWindow::on_registerButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loginButton_clicked()
{
    showLoginForm();
}

void MainWindow::on_registerButton_clicked()
{
    showRegisterForm();
}

void MainWindow::showLoginForm()
{
    if (!loginForm) {
        loginForm = new LoginForm(nullptr);
        loginForm->setMainWindow(this);
        connect(loginForm, &LoginForm::loginSuccess, this, &MainWindow::handleLoginSuccess);
        connect(loginForm, &LoginForm::registerLinkClicked, this, &MainWindow::showRegisterForm);
        connect(loginForm, &LoginForm::backClicked, this, &MainWindow::handleBackToMain);
    }
    loginForm->show();
    this->hide();
}

void MainWindow::showRegisterForm()
{
    if (!registerForm) {
        registerForm = new RegisterForm(nullptr);
        registerForm->setMainWindow(this);
        connect(registerForm, &RegisterForm::registerSuccess, this, &MainWindow::handleRegisterSuccess);
        connect(registerForm, &RegisterForm::backClicked, this, &MainWindow::handleBackToMain);
    }
    registerForm->show();
    if (loginForm) loginForm->hide();
    this->hide();
}

void MainWindow::handleLoginSuccess(const User& user)
{
    currentUser = user;
    if (loginForm) loginForm->hide();
    if (user.role == "user") {
        if (!userDashboard) {
            userDashboard = new UserDashboard(nullptr);
            connect(userDashboard, &UserDashboard::logoutClicked, this, &MainWindow::handleLogout);
        }
        userDashboard->setUser(user);
        userDashboard->show();
    } else if (user.role == "receptionist") {
        if (!receptionistDashboard) {
            receptionistDashboard = new ReceptionistDashboard(nullptr);
            connect(receptionistDashboard, &ReceptionistDashboard::logoutClicked, this, &MainWindow::handleLogout);
            connect(receptionistDashboard->findChild<QTabWidget*>("tabWidget"), &QTabWidget::currentChanged, [=](int idx){
                if (idx == 1) receptionistDashboard->on_guestManagementTab_selected();
                else if (idx == 2) receptionistDashboard->on_reservationTable_itemSelectionChanged();
                else if (idx == 3) receptionistDashboard->on_roomStatusTab_selected();
            });
        }
        receptionistDashboard->setUser(user);
        receptionistDashboard->show();
    } else if (user.role == "admin") {
        if (!adminDashboard) {
            adminDashboard = new AdminDashboard(nullptr);
            connect(adminDashboard, &AdminDashboard::logoutClicked, this, &MainWindow::handleLogout);
            connect(adminDashboard->findChild<QTabWidget*>("tabWidget"), &QTabWidget::currentChanged, [=](int idx){
                if (idx == 0) adminDashboard->on_userTab_selected();
                else if (idx == 1) adminDashboard->on_roomTab_selected();
            });
        }
        adminDashboard->setUser(user);
        adminDashboard->show();
    }
}

void MainWindow::handleRegisterSuccess(const User& user)
{
    (void)user;
    if (registerForm) registerForm->hide();
    showLoginForm();
}

void MainWindow::handleLogout()
{
    if (userDashboard) userDashboard->hide();
    if (receptionistDashboard) receptionistDashboard->hide();
    if (adminDashboard) adminDashboard->hide();
    this->show();
}

void MainWindow::handleBackToMain()
{
    if (loginForm) loginForm->hide();
    if (registerForm) registerForm->hide();
    this->show();
} 