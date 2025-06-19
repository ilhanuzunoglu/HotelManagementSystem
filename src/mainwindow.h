#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class LoginForm;
class RegisterForm;
class UserDashboard;
class ReceptionistDashboard;
class AdminDashboard;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_loginButton_clicked();
    void on_registerButton_clicked();

private slots:
    void showLoginForm();
    void showRegisterForm();
    void handleLoginSuccess(const User& user);
    void handleRegisterSuccess(const User& user);
    void handleLogout();
    void handleBackToMain();

private:
    Ui::MainWindow *ui;
    LoginForm* loginForm;
    RegisterForm* registerForm;
    UserDashboard* userDashboard;
    ReceptionistDashboard* receptionistDashboard;
    AdminDashboard* adminDashboard;
    User currentUser;
};

#endif // MAINWINDOW_H 