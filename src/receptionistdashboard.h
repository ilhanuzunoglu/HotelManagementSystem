// receptionistdashboard.h
#ifndef RECEPTIONISTDASHBOARD_H
#define RECEPTIONISTDASHBOARD_H

#include <QWidget>
#include "database.h"

namespace Ui { class ReceptionistDashboard; }
class MainWindow;

class ReceptionistDashboard : public QWidget
{
    Q_OBJECT
public:
    explicit ReceptionistDashboard(QWidget *parent = nullptr);
    ~ReceptionistDashboard();
    void setUser(const User& user);

signals:
    void logoutClicked();

private slots:
    void on_logoutButton_clicked();

private:
    Ui::ReceptionistDashboard *ui;
    MainWindow* mainWindow;
    User currentUser;
};

#endif // RECEPTIONISTDASHBOARD_H 