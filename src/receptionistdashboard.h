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

public slots:
    void on_guestManagementTab_selected();
    void on_reservationTable_itemSelectionChanged();
    void on_roomStatusTab_selected();

private slots:
    void on_logoutButton_clicked();
    void on_approveReservationButton_clicked();
    void on_checkInButton_clicked();
    void on_checkOutButton_clicked();
    void on_cancelReservationButton_clicked();
    void on_addGuestButton_clicked();
    void on_editGuestButton_clicked();
    void on_deleteGuestButton_clicked();
    void on_updateRoomStatusButton_clicked();
    void populateBillingTable();
    void on_processPaymentButton_clicked();

private:
    void populateReservationTable();
    void updateReservationButtons();
    void populateRoomStatusTable();
    void populateGuestTable();
    Ui::ReceptionistDashboard *ui;
    MainWindow* mainWindow;
    User currentUser;
    int selectedReservationId = -1;
};

#endif // RECEPTIONISTDASHBOARD_H 