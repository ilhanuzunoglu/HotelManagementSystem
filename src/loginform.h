// loginform.h
#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>

namespace Ui { class LoginForm; }

class MainWindow;
class UserDashboard;
class ReceptionistDashboard;
class AdminDashboard;

class LoginForm : public QWidget
{
    Q_OBJECT
public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

private slots:
    void on_loginButton_clicked();
    void on_registerLinkButton_clicked();
    void on_backButton_clicked();

private:
    Ui::LoginForm *ui;
    MainWindow* mainWindow;
    UserDashboard* userDashboard;
    ReceptionistDashboard* receptionistDashboard;
    AdminDashboard* adminDashboard;
};

#endif // LOGINFORM_H 