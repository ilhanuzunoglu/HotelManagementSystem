// userdashboard.h
#ifndef USERDASHBOARD_H
#define USERDASHBOARD_H

#include <QWidget>
#include "database.h"

namespace Ui { class UserDashboard; }
class MainWindow;

class UserDashboard : public QWidget
{
    Q_OBJECT
public:
    explicit UserDashboard(QWidget *parent = nullptr);
    ~UserDashboard();
    void setUser(const User& user);

signals:
    void logoutClicked();

private slots:
    void on_logoutButton_clicked();
    void on_createReservationButton_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_roomTypeOrDateChanged();

private:
    void populateReservationsTable();
    void populateRoomTypeComboBox();
    void updateAvailableRooms();
    void populateAvailableRoomNumbers();
    void clearNewReservationForm();
    Ui::UserDashboard *ui;
    MainWindow* mainWindow;
    User currentUser;
};

#endif // USERDASHBOARD_H 