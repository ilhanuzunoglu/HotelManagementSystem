// loginform.h
#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include "database.h"

namespace Ui { class LoginForm; }

class MainWindow;

class LoginForm : public QWidget
{
    Q_OBJECT
public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();
    void setMainWindow(MainWindow* mw);
    void clearFields();

signals:
    void loginSuccess(const User& user);
    void registerLinkClicked();
    void backClicked();

private slots:
    void on_loginButton_clicked();
    void on_registerLinkButton_clicked();
    void on_backButton_clicked();
    void on_showPasswordButton_clicked();

private:
    Ui::LoginForm *ui;
    MainWindow* mainWindow;
};

#endif // LOGINFORM_H 