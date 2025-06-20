// receptionistdashboard.cpp 
#include "receptionistdashboard.h"
#include "ui_receptionistdashboard.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QTableWidget>
#include <QPushButton>
#include <QSqlQuery>
#include <QInputDialog>
#include "invoicedialog.h"
#include "usereditdialog.h"

ReceptionistDashboard::ReceptionistDashboard(QWidget *parent)
    : QWidget(parent), ui(new Ui::ReceptionistDashboard), mainWindow(nullptr)
{
    ui->setupUi(this);
    connect(ui->logoutButton, &QPushButton::clicked, this, &ReceptionistDashboard::on_logoutButton_clicked);
    connect(ui->approveReservationButton, &QPushButton::clicked, this, &ReceptionistDashboard::on_approveReservationButton_clicked);
    connect(ui->checkInButton, &QPushButton::clicked, this, &ReceptionistDashboard::on_checkInButton_clicked);
    connect(ui->checkOutButton, &QPushButton::clicked, this, &ReceptionistDashboard::on_checkOutButton_clicked);
    connect(ui->cancelReservationButton, &QPushButton::clicked, this, &ReceptionistDashboard::on_cancelReservationButton_clicked);
    connect(ui->reservationTable, &QTableWidget::itemSelectionChanged, this, &ReceptionistDashboard::on_reservationTable_itemSelectionChanged);
    connect(ui->addGuestButton, &QPushButton::clicked, this, &ReceptionistDashboard::on_addGuestButton_clicked);
    connect(ui->editGuestButton, &QPushButton::clicked, this, &ReceptionistDashboard::on_editGuestButton_clicked);
    connect(ui->deleteGuestButton, &QPushButton::clicked, this, &ReceptionistDashboard::on_deleteGuestButton_clicked);
    connect(ui->updateRoomStatusButton, &QPushButton::clicked, this, &ReceptionistDashboard::on_updateRoomStatusButton_clicked);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int idx) {
        if (ui->tabWidget->widget(idx)->objectName() == "roomStatusTab")
            on_roomStatusTab_selected();
        if (ui->tabWidget->widget(idx)->objectName() == "billingTab")
            populateBillingTable();
    });
    connect(ui->processPaymentButton, &QPushButton::clicked, this, &ReceptionistDashboard::on_processPaymentButton_clicked);
}

ReceptionistDashboard::~ReceptionistDashboard() { delete ui; }

void ReceptionistDashboard::setUser(const User& user)
{
    currentUser = user;
    ui->welcomeLabel->setText("Hoş Geldiniz, " + user.username);
    populateReservationTable();
    updateReservationButtons();
}

void ReceptionistDashboard::populateReservationTable()
{
    QList<Reservation> reservations = Database::instance().getAllReservations();
    ui->reservationTable->setRowCount(reservations.size());
    ui->reservationTable->setColumnCount(7);
    QStringList headers = {"ID", "Misafir", "Oda No", "Giriş", "Çıkış", "Durum", "Oda Tipi"};
    ui->reservationTable->setHorizontalHeaderLabels(headers);
    QList<Room> allRooms = Database::instance().getAllRooms();
    for (int i = 0; i < reservations.size(); ++i) {
        const Reservation& r = reservations[i];
        Room room;
        for (const Room& rm : allRooms) {
            if (rm.room_no == r.room_no) { room = rm; break; }
        }
        ui->reservationTable->setItem(i, 0, new QTableWidgetItem(QString::number(r.id)));
        ui->reservationTable->setItem(i, 1, new QTableWidgetItem(r.username));
        ui->reservationTable->setItem(i, 2, new QTableWidgetItem(QString::number(r.room_no)));
        ui->reservationTable->setItem(i, 3, new QTableWidgetItem(r.checkin));
        ui->reservationTable->setItem(i, 4, new QTableWidgetItem(r.checkout));
        ui->reservationTable->setItem(i, 5, new QTableWidgetItem(r.status));
        ui->reservationTable->setItem(i, 6, new QTableWidgetItem(room.type));
    }
    ui->reservationTable->resizeColumnsToContents();
    selectedReservationId = -1;
}

void ReceptionistDashboard::updateReservationButtons()
{
    // Enable/disable buttons based on selected reservation status
    bool hasSelection = selectedReservationId != -1;
    ui->approveReservationButton->setEnabled(false);
    ui->checkInButton->setEnabled(false);
    ui->checkOutButton->setEnabled(false);
    ui->cancelReservationButton->setEnabled(false);
    if (!hasSelection) return;
    int row = ui->reservationTable->currentRow();
    QString status = ui->reservationTable->item(row, 5)->text();
    if (status == "pending") ui->approveReservationButton->setEnabled(true);
    if (status == "approved") ui->checkInButton->setEnabled(true);
    if (status == "checked_in") ui->checkOutButton->setEnabled(true);
    if (status == "pending" || status == "approved") ui->cancelReservationButton->setEnabled(true);
}

void ReceptionistDashboard::on_reservationTable_itemSelectionChanged()
{
    int row = ui->reservationTable->currentRow();
    if (row >= 0) {
        selectedReservationId = ui->reservationTable->item(row, 0)->text().toInt();
    } else {
        selectedReservationId = -1;
    }
    updateReservationButtons();
}

void ReceptionistDashboard::on_approveReservationButton_clicked()
{
    if (selectedReservationId == -1) return;
    if (Database::instance().updateReservationStatus(selectedReservationId, "approved")) {
        QMessageBox::information(this, "Başarılı", "Rezervasyon onaylandı.");
        populateReservationTable();
    } else {
        QMessageBox::warning(this, "Hata", "Rezervasyon onaylanamadı.");
    }
}

void ReceptionistDashboard::on_checkInButton_clicked()
{
    if (selectedReservationId == -1) return;
    if (Database::instance().updateReservationStatus(selectedReservationId, "checked_in")) {
        QMessageBox::information(this, "Başarılı", "Check-in yapıldı.");
        populateReservationTable();
    } else {
        QMessageBox::warning(this, "Hata", "Check-in yapılamadı.");
    }
}

void ReceptionistDashboard::on_checkOutButton_clicked()
{
    if (selectedReservationId == -1) return;
    if (Database::instance().updateReservationStatus(selectedReservationId, "checked_out")) {
        QMessageBox::information(this, "Başarılı", "Check-out yapıldı.");
        populateReservationTable();
    } else {
        QMessageBox::warning(this, "Hata", "Check-out yapılamadı.");
    }
}

void ReceptionistDashboard::on_cancelReservationButton_clicked()
{
    if (selectedReservationId == -1) return;
    if (Database::instance().updateReservationStatus(selectedReservationId, "cancelled")) {
        QMessageBox::information(this, "Başarılı", "Rezervasyon iptal edildi.");
        populateReservationTable();
    } else {
        QMessageBox::warning(this, "Hata", "Rezervasyon iptal edilemedi.");
    }
}

void ReceptionistDashboard::on_logoutButton_clicked()
{
    emit logoutClicked();
}

void ReceptionistDashboard::populateRoomStatusTable()
{
    QList<Room> rooms = Database::instance().getAllRooms();
    ui->roomStatusTable->setRowCount(rooms.size());
    ui->roomStatusTable->setColumnCount(4);
    QStringList headers = {"Oda No", "Kat", "Tip", "Durum"};
    ui->roomStatusTable->setHorizontalHeaderLabels(headers);
    for (int i = 0; i < rooms.size(); ++i) {
        const Room& r = rooms[i];
        ui->roomStatusTable->setItem(i, 0, new QTableWidgetItem(QString::number(r.room_no)));
        ui->roomStatusTable->setItem(i, 1, new QTableWidgetItem(QString::number(r.floor)));
        ui->roomStatusTable->setItem(i, 2, new QTableWidgetItem(r.type));
        ui->roomStatusTable->setItem(i, 3, new QTableWidgetItem(r.status));
    }
    ui->roomStatusTable->resizeColumnsToContents();
}

void ReceptionistDashboard::populateGuestTable()
{
    QSqlQuery query("SELECT name, surname, email, username FROM users WHERE role = 'user'");
    int row = 0;
    ui->guestTable->setRowCount(0);
    while (query.next()) {
        ui->guestTable->insertRow(row);
        for (int col = 0; col < 4; ++col) {
            ui->guestTable->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
        ++row;
    }
    ui->guestTable->resizeColumnsToContents();
}

void ReceptionistDashboard::on_roomStatusTab_selected()
{
    populateRoomStatusTable();
}

void ReceptionistDashboard::on_guestManagementTab_selected()
{
    populateGuestTable();
}

void ReceptionistDashboard::on_addGuestButton_clicked() {
    ui->addGuestButton->setEnabled(false);
    UserEditDialog dialog(this);
    dialog.setRoleSelectionVisible(false);
    User user;
    user.role = "user";
    dialog.setUser(user);
    dialog.setEditMode(false);
    if (dialog.exec() == QDialog::Accepted) {
        User newUser = dialog.getUser();
        newUser.role = "user";
        QString errorMsg;
        if (Database::instance().addUser(newUser, errorMsg)) {
            QMessageBox::information(this, "Başarılı", "Kullanıcı eklendi.");
            populateGuestTable();
        } else {
            QMessageBox::warning(this, "Hata", errorMsg);
        }
    }
    ui->addGuestButton->setEnabled(true);
}

void ReceptionistDashboard::on_editGuestButton_clicked() {
    ui->editGuestButton->setEnabled(false);
    int row = ui->guestTable->currentRow();
    if (row < 0) { ui->editGuestButton->setEnabled(true); return; }
    QString username = ui->guestTable->item(row, 3)->text();
    User user = Database::instance().getUser(username);
    UserEditDialog dialog(this);
    dialog.setRoleSelectionVisible(false);
    dialog.setUser(user);
    dialog.setEditMode(true);
    if (dialog.exec() == QDialog::Accepted) {
        User updated = dialog.getUser();
        updated.role = "user";
        QString errorMsg;
        if (Database::instance().editUser(updated, errorMsg)) {
            QMessageBox::information(this, "Başarılı", "Kullanıcı güncellendi.");
            populateGuestTable();
        } else {
            QMessageBox::warning(this, "Hata", errorMsg);
        }
    }
    ui->editGuestButton->setEnabled(true);
}

void ReceptionistDashboard::on_deleteGuestButton_clicked() {
    ui->deleteGuestButton->setEnabled(false);
    int row = ui->guestTable->currentRow();
    if (row < 0) { ui->deleteGuestButton->setEnabled(true); return; }
    QString username = ui->guestTable->item(row, 3)->text();
    if (QMessageBox::question(this, "Sil", "Kullanıcı silinsin mi?") == QMessageBox::Yes) {
        QString errorMsg;
        if (Database::instance().deleteUser(username, errorMsg)) {
            QMessageBox::information(this, "Başarılı", "Kullanıcı silindi.");
            populateGuestTable();
        } else {
            QMessageBox::warning(this, "Hata", errorMsg);
        }
    }
    ui->deleteGuestButton->setEnabled(true);
}

void ReceptionistDashboard::on_updateRoomStatusButton_clicked() {
    int row = ui->roomStatusTable->currentRow();
    if (row < 0) return;
    int room_no = ui->roomStatusTable->item(row, 0)->text().toInt();
    QString currentStatus = ui->roomStatusTable->item(row, 3)->text();
    QString newStatus = QInputDialog::getText(this, "Oda Durumu Güncelle", "Yeni Durum:", QLineEdit::Normal, currentStatus);
    if (newStatus.isEmpty() || newStatus == currentStatus) return;
    Room room;
    QList<Room> allRooms = Database::instance().getAllRooms();
    for (const Room& r : allRooms) {
        if (r.room_no == room_no) { room = r; break; }
    }
    room.status = newStatus;
    QString errorMsg;
    if (Database::instance().editRoom(room, errorMsg)) {
        QMessageBox::information(this, "Başarılı", "Oda durumu güncellendi.");
        populateRoomStatusTable();
    } else {
        QMessageBox::warning(this, "Hata", errorMsg);
    }
}

void ReceptionistDashboard::populateBillingTable()
{
    QList<Reservation> reservations = Database::instance().getAllReservations();
    ui->billingTable->setRowCount(0);
    for (const Reservation& r : reservations) {
        if (r.payment_status == "paid") {
            int row = ui->billingTable->rowCount();
            ui->billingTable->insertRow(row);
            ui->billingTable->setItem(row, 0, new QTableWidgetItem(r.username));
            ui->billingTable->setItem(row, 1, new QTableWidgetItem(QString::number(r.room_no)));
            ui->billingTable->setItem(row, 2, new QTableWidgetItem(QString::number(r.price, 'f', 2)));
            ui->billingTable->setItem(row, 3, new QTableWidgetItem(r.status));
        }
    }
    ui->billingTable->resizeColumnsToContents();
}

void ReceptionistDashboard::on_processPaymentButton_clicked()
{
    int row = ui->billingTable->currentRow();
    if (row < 0) return;
    int room_no = ui->billingTable->item(row, 1)->text().toInt();
    double price = ui->billingTable->item(row, 2)->text().toDouble();
    QString username = ui->billingTable->item(row, 0)->text();
    QList<Reservation> reservations = Database::instance().getAllReservations();
    Reservation selected;
    for (const Reservation& r : reservations) {
        if (r.username == username && r.room_no == room_no && r.price == price) {
            selected = r;
            break;
        }
    }
    InvoiceDialog dialog(selected, this);
    dialog.exec();
} 