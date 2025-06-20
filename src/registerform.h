// registerform.h
#ifndef REGISTERFORM_H
#define REGISTERFORM_H

#include <QWidget>
#include "database.h"
#include <QLineEdit>

namespace Ui { class RegisterForm; }

class MainWindow;
class LoginForm;

class RegisterForm : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterForm(QWidget *parent = nullptr);
    ~RegisterForm();
    void setMainWindow(MainWindow* mw);

signals:
    void registerSuccess(const User& user);
    void backClicked();

private slots:
    void on_registerButton_clicked();
    void on_backButton_clicked();

private:
    Ui::RegisterForm *ui;
    MainWindow* mainWindow;
    LoginForm* loginForm;
    QLineEdit* phoneLineEdit;
};

#endif // REGISTERFORM_H 