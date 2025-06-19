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

public slots:
    void on_userTab_selected();
    void on_roomTab_selected();

private slots:
    void on_logoutButton_clicked();
    void on_addUserButton_clicked();
    void on_editUserButton_clicked();
    void on_deleteUserButton_clicked();
    void on_addRoomButton_clicked();
    void on_editRoomButton_clicked();
    void on_deleteRoomButton_clicked();
    void on_addPriceButton_clicked();
    void on_editPriceButton_clicked();
    void on_deletePriceButton_clicked();

private:
    void populateRoomTable();
    void populateUserTable();
    void populatePricingTable();
    Ui::AdminDashboard *ui;
    MainWindow* mainWindow;
    User currentUser;
};

#endif // ADMINDASHBOARD_H 