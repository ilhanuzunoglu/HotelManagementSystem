// userdashboard.h
#ifndef USERDASHBOARD_H
#define USERDASHBOARD_H

#include <QWidget>

namespace Ui { class UserDashboard; }
class MainWindow;

class UserDashboard : public QWidget
{
    Q_OBJECT
public:
    explicit UserDashboard(QWidget *parent = nullptr);
    ~UserDashboard();

private slots:
    void on_logoutButton_clicked();

private:
    Ui::UserDashboard *ui;
    MainWindow* mainWindow;
};

#endif // USERDASHBOARD_H 