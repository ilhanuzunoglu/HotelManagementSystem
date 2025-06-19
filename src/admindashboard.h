// admindashboard.h
#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QWidget>
#include "database.h"

namespace Ui { class AdminDashboard; }
class MainWindow;

class AdminDashboard : public QWidget
{
    Q_OBJECT
public:
    explicit AdminDashboard(QWidget *parent = nullptr);
    ~AdminDashboard();
    void setUser(const User& user);

signals:
    void logoutClicked();

private slots:
    void on_logoutButton_clicked();

private:
    Ui::AdminDashboard *ui;
    MainWindow* mainWindow;
    User currentUser;
};

#endif // ADMINDASHBOARD_H 