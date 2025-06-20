// admindashboard.cpp 
#include "admindashboard.h"
#include "ui_admindashboard.h"
#include "mainwindow.h"
#include <QTableWidget>
#include <QSqlQuery>
#include <QInputDialog>
#include <QMessageBox>
#include "usereditdialog.h"
#include "roomeditdialog.h"
#include <QLineEdit>

AdminDashboard::AdminDashboard(QWidget *parent)
    : QWidget(parent), ui(new Ui::AdminDashboard), mainWindow(nullptr)
{
    ui->setupUi(this);
    connect(ui->logoutButton, &QPushButton::clicked, this, &AdminDashboard::on_logoutButton_clicked);
    connect(ui->updateSettingsButton, &QPushButton::clicked, this, &AdminDashboard::on_updateSettingsButton_clicked);
    connect(ui->pricingTable, &QTableWidget::itemSelectionChanged, this, &AdminDashboard::on_pricingTable_itemSelectionChanged);
    populatePricingTable();
}

AdminDashboard::~AdminDashboard() { delete ui; }

void AdminDashboard::setUser(const User& user)
{
    currentUser = user;
    ui->welcomeLabel->setText("Hoş Geldiniz, " + user.username);
    populateRoomTable();
    populateUserTable();
    loadSettingsToUI();
}

void AdminDashboard::populateRoomTable()
{
    QList<Room> rooms = Database::instance().getAllRooms();
    ui->roomTable->setRowCount(rooms.size());
    ui->roomTable->setColumnCount(4);
    QStringList headers = {"Oda No", "Tip", "Kat", "Durum"};
    ui->roomTable->setHorizontalHeaderLabels(headers);
    for (int i = 0; i < rooms.size(); ++i) {
        const Room& r = rooms[i];
        ui->roomTable->setItem(i, 0, new QTableWidgetItem(QString::number(r.room_no)));
        ui->roomTable->setItem(i, 1, new QTableWidgetItem(r.type));
        ui->roomTable->setItem(i, 2, new QTableWidgetItem(QString::number(r.floor)));
        ui->roomTable->setItem(i, 3, new QTableWidgetItem(r.status));
    }
    ui->roomTable->resizeColumnsToContents();
}

void AdminDashboard::populateUserTable()
{
    QSqlQuery query("SELECT name, surname, email, role, username FROM users");
    int row = 0;
    ui->userTable->setRowCount(0);
    while (query.next()) {
        ui->userTable->insertRow(row);
        for (int col = 0; col < 5; ++col) {
            ui->userTable->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
        ++row;
    }
    ui->userTable->resizeColumnsToContents();
}

void AdminDashboard::populatePricingTable()
{
    QStringList types = {"standart", "suit", "deluxe"};
    ui->pricingTable->setRowCount(3);
    ui->pricingTable->setColumnCount(1);
    for (int i = 0; i < 3; ++i) {
        double price = Database::instance().getPriceForRoomType(types[i]);
        if (price <= 0.0) price = Database::instance().getDefaultPriceForRoomType(types[i]);
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(price, 'f', 2));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        priceItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->pricingTable->setItem(i, 0, priceItem);
    }
    ui->pricingTable->setHorizontalHeaderLabels({"Fiyat (TL)"});
    ui->pricingTable->resizeColumnsToContents();
    ui->priceEdit->clear();
}

void AdminDashboard::loadSettingsToUI()
{
    Settings s = Database::instance().getSettings();
    ui->hotelNameEdit->setText(s.hotel_name);
    ui->hotelAddressEdit->setText(s.hotel_address);
    ui->hotelContactEdit->setText(s.hotel_contact);
}

void AdminDashboard::on_logoutButton_clicked()
{
    emit logoutClicked();
}

void AdminDashboard::on_roomTab_selected()
{
    populateRoomTable();
}

void AdminDashboard::on_userTab_selected()
{
    populateUserTable();
}

void AdminDashboard::on_addUserButton_clicked() {
    ui->addUserButton->setEnabled(false);
    UserEditDialog dialog(this);
    dialog.setEditMode(false);
    if (dialog.exec() == QDialog::Accepted) {
        User user = dialog.getUser();
        QString errorMsg;
        if (Database::instance().addUser(user, errorMsg)) {
            QMessageBox::information(this, "Başarılı", "Kullanıcı eklendi.");
            populateUserTable();
        } else {
            QMessageBox::warning(this, "Hata", errorMsg);
        }
    }
    ui->addUserButton->setEnabled(true);
}

void AdminDashboard::on_editUserButton_clicked() {
    ui->editUserButton->setEnabled(false);
    int row = ui->userTable->currentRow();
    if (row < 0) { ui->editUserButton->setEnabled(true); return; }
    QString username = ui->userTable->item(row, 4)->text();
    User user = Database::instance().getUser(username);
    UserEditDialog dialog(this);
    dialog.setUser(user);
    dialog.setEditMode(true);
    if (dialog.exec() == QDialog::Accepted) {
        User updated = dialog.getUser();
        QString errorMsg;
        if (Database::instance().editUser(updated, errorMsg)) {
            QMessageBox::information(this, "Başarılı", "Kullanıcı güncellendi.");
            populateUserTable();
        } else {
            QMessageBox::warning(this, "Hata", errorMsg);
        }
    }
    ui->editUserButton->setEnabled(true);
}

void AdminDashboard::on_deleteUserButton_clicked() {
    ui->deleteUserButton->setEnabled(false);
    int row = ui->userTable->currentRow();
    if (row < 0) { ui->deleteUserButton->setEnabled(true); return; }
    QString username = ui->userTable->item(row, 4)->text();
    if (QMessageBox::question(this, "Sil", "Kullanıcı silinsin mi?") == QMessageBox::Yes) {
        QString errorMsg;
        if (Database::instance().deleteUser(username, errorMsg)) {
            QMessageBox::information(this, "Başarılı", "Kullanıcı silindi.");
            populateUserTable();
        } else {
            QMessageBox::warning(this, "Hata", errorMsg);
        }
    }
    ui->deleteUserButton->setEnabled(true);
}

void AdminDashboard::on_addRoomButton_clicked() {
    ui->addRoomButton->setEnabled(false);
    RoomEditDialog dialog(this);
    dialog.setEditMode(false);
    if (dialog.exec() == QDialog::Accepted) {
        Room room = dialog.getRoom();
        QString errorMsg;
        if (Database::instance().addRoom(room, errorMsg)) {
            QMessageBox::information(this, "Başarılı", "Oda eklendi.");
            populateRoomTable();
        } else {
            QMessageBox::warning(this, "Hata", errorMsg);
        }
    }
    ui->addRoomButton->setEnabled(true);
}

void AdminDashboard::on_editRoomButton_clicked() {
    ui->editRoomButton->setEnabled(false);
    int row = ui->roomTable->currentRow();
    if (row < 0) { ui->editRoomButton->setEnabled(true); return; }
    Room room;
    room.room_no = ui->roomTable->item(row, 0)->text().toInt();
    room.type = ui->roomTable->item(row, 1)->text();
    room.floor = ui->roomTable->item(row, 2)->text().toInt();
    room.status = ui->roomTable->item(row, 3)->text();
    RoomEditDialog dialog(this);
    dialog.setRoom(room);
    dialog.setEditMode(true);
    if (dialog.exec() == QDialog::Accepted) {
        Room updated = dialog.getRoom();
        QString errorMsg;
        if (Database::instance().editRoom(updated, errorMsg)) {
            QMessageBox::information(this, "Başarılı", "Oda güncellendi.");
            populateRoomTable();
        } else {
            QMessageBox::warning(this, "Hata", errorMsg);
        }
    }
    ui->editRoomButton->setEnabled(true);
}

void AdminDashboard::on_deleteRoomButton_clicked() {
    ui->deleteRoomButton->setEnabled(false);
    int row = ui->roomTable->currentRow();
    if (row < 0) { ui->deleteRoomButton->setEnabled(true); return; }
    int room_no = ui->roomTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Sil", "Oda silinsin mi?") == QMessageBox::Yes) {
        QString errorMsg;
        if (Database::instance().deleteRoom(room_no, errorMsg)) {
            QMessageBox::information(this, "Başarılı", "Oda silindi.");
            populateRoomTable();
        } else {
            QMessageBox::warning(this, "Hata", errorMsg);
        }
    }
    ui->deleteRoomButton->setEnabled(true);
}

void AdminDashboard::on_updateSettingsButton_clicked()
{
    Settings s;
    s.hotel_name = ui->hotelNameEdit->text();
    s.hotel_address = ui->hotelAddressEdit->text();
    s.hotel_contact = ui->hotelContactEdit->text();
    if (Database::instance().updateSettings(s)) {
        QMessageBox::information(this, "Başarılı", "Sistem ayarları güncellendi.");
    } else {
        QMessageBox::warning(this, "Hata", "Ayarlar güncellenemedi.");
    }
}

void AdminDashboard::on_pricingTable_itemSelectionChanged() {
    int row = ui->pricingTable->currentRow();
    if (row < 0 || !ui->pricingTable->item(row, 0)) {
        ui->priceEdit->clear();
        return;
    }
    ui->priceEdit->setText(ui->pricingTable->item(row, 0)->text());
}

void AdminDashboard::on_editPriceButton_clicked() {
    int row = ui->pricingTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Hata", "Lütfen bir oda tipi seçin.");
        return;
    }
    QString priceStr = ui->priceEdit->text().replace(',', '.');
    bool ok = false;
    double newPrice = priceStr.toDouble(&ok);
    if (!ok || newPrice <= 0) {
        QMessageBox::warning(this, "Hata", "Geçerli bir fiyat girin.");
        return;
    }
    QStringList types = {"standart", "suit", "deluxe"};
    QString type = types[row];
    Price priceObj;
    priceObj.room_type = type;
    priceObj.price = newPrice;
    QString errorMsg;
    if (!Database::instance().editPrice(priceObj, errorMsg)) {
        if (!Database::instance().addPrice(priceObj, errorMsg)) {
            QMessageBox::warning(this, "Hata", errorMsg);
            return;
        }
    }
    QMessageBox::information(this, "Başarılı", "Fiyat güncellendi.");
    populatePricingTable();
    ui->pricingTable->setCurrentCell(row, 0);
    on_pricingTable_itemSelectionChanged();
} 