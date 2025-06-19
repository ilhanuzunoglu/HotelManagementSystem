// admindashboard.cpp 
#include "admindashboard.h"
#include "ui_admindashboard.h"
#include "mainwindow.h"
#include <QTableWidget>
#include <QSqlQuery>
#include <QInputDialog>
#include <QMessageBox>

AdminDashboard::AdminDashboard(QWidget *parent)
    : QWidget(parent), ui(new Ui::AdminDashboard), mainWindow(nullptr)
{
    ui->setupUi(this);
    connect(ui->logoutButton, &QPushButton::clicked, this, &AdminDashboard::on_logoutButton_clicked);
}

AdminDashboard::~AdminDashboard() { delete ui; }

void AdminDashboard::setUser(const User& user)
{
    currentUser = user;
    ui->welcomeLabel->setText("Hoş Geldiniz, " + user.username);
    populateRoomTable();
    populateUserTable();
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
    QList<Price> prices = Database::instance().getAllPrices();
    ui->pricingTable->setRowCount(prices.size());
    ui->pricingTable->setColumnCount(2);
    QStringList headers = {"Oda Tipi", "Fiyat"};
    ui->pricingTable->setHorizontalHeaderLabels(headers);
    for (int i = 0; i < prices.size(); ++i) {
        const Price& p = prices[i];
        ui->pricingTable->setItem(i, 0, new QTableWidgetItem(p.room_type));
        ui->pricingTable->setItem(i, 1, new QTableWidgetItem(QString::number(p.price, 'f', 2)));
    }
    ui->pricingTable->resizeColumnsToContents();
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
    User user;
    user.username = QInputDialog::getText(this, "Kullanıcı Adı", "Kullanıcı Adı:");
    if (user.username.isEmpty()) { ui->addUserButton->setEnabled(true); return; }
    user.password = QInputDialog::getText(this, "Şifre", "Şifre:");
    user.role = QInputDialog::getText(this, "Rol", "Rol:", QLineEdit::Normal, "user");
    user.name = QInputDialog::getText(this, "Ad", "Ad:");
    user.surname = QInputDialog::getText(this, "Soyad", "Soyad:");
    user.email = QInputDialog::getText(this, "E-posta", "E-posta:");
    QString errorMsg;
    if (Database::instance().addUser(user, errorMsg)) {
        QMessageBox::information(this, "Başarılı", "Kullanıcı eklendi.");
        populateUserTable();
    } else {
        QMessageBox::warning(this, "Hata", errorMsg);
    }
    ui->addUserButton->setEnabled(true);
}

void AdminDashboard::on_editUserButton_clicked() {
    ui->editUserButton->setEnabled(false);
    int row = ui->userTable->currentRow();
    if (row < 0) { ui->editUserButton->setEnabled(true); return; }
    QString username = ui->userTable->item(row, 4)->text();
    User user = Database::instance().getUser(username);
    user.name = QInputDialog::getText(this, "Ad", "Ad:", QLineEdit::Normal, user.name);
    user.surname = QInputDialog::getText(this, "Soyad", "Soyad:", QLineEdit::Normal, user.surname);
    user.email = QInputDialog::getText(this, "E-posta", "E-posta:", QLineEdit::Normal, user.email);
    user.password = QInputDialog::getText(this, "Şifre", "Şifre:", QLineEdit::Normal, user.password);
    user.role = QInputDialog::getText(this, "Rol", "Rol:", QLineEdit::Normal, user.role);
    QString errorMsg;
    if (Database::instance().editUser(user, errorMsg)) {
        QMessageBox::information(this, "Başarılı", "Kullanıcı güncellendi.");
        populateUserTable();
    } else {
        QMessageBox::warning(this, "Hata", errorMsg);
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
    Room room;
    room.room_no = QInputDialog::getInt(this, "Oda No", "Oda No:");
    room.floor = QInputDialog::getInt(this, "Kat", "Kat:");
    room.type = QInputDialog::getText(this, "Oda Tipi", "Oda Tipi:");
    room.status = "available";
    QString errorMsg;
    if (Database::instance().addRoom(room, errorMsg)) {
        QMessageBox::information(this, "Başarılı", "Oda eklendi.");
        populateRoomTable();
    } else {
        QMessageBox::warning(this, "Hata", errorMsg);
    }
    ui->addRoomButton->setEnabled(true);
}

void AdminDashboard::on_editRoomButton_clicked() {
    ui->editRoomButton->setEnabled(false);
    int row = ui->roomTable->currentRow();
    if (row < 0) { ui->editRoomButton->setEnabled(true); return; }
    Room room;
    room.room_no = ui->roomTable->item(row, 0)->text().toInt();
    room.type = QInputDialog::getText(this, "Oda Tipi", "Oda Tipi:", QLineEdit::Normal, ui->roomTable->item(row, 1)->text());
    room.floor = QInputDialog::getInt(this, "Kat", "Kat:", ui->roomTable->item(row, 2)->text().toInt());
    room.status = QInputDialog::getText(this, "Durum", "Durum:", QLineEdit::Normal, ui->roomTable->item(row, 3)->text());
    QString errorMsg;
    if (Database::instance().editRoom(room, errorMsg)) {
        QMessageBox::information(this, "Başarılı", "Oda güncellendi.");
        populateRoomTable();
    } else {
        QMessageBox::warning(this, "Hata", errorMsg);
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

void AdminDashboard::on_addPriceButton_clicked() {
    ui->addPriceButton->setEnabled(false);
    Price price;
    price.room_type = QInputDialog::getText(this, "Oda Tipi", "Oda Tipi:");
    price.price = QInputDialog::getDouble(this, "Fiyat", "Fiyat:");
    QString errorMsg;
    if (Database::instance().addPrice(price, errorMsg)) {
        QMessageBox::information(this, "Başarılı", "Fiyat eklendi.");
        populatePricingTable();
    } else {
        QMessageBox::warning(this, "Hata", errorMsg);
    }
    ui->addPriceButton->setEnabled(true);
}

void AdminDashboard::on_editPriceButton_clicked() {
    ui->editPriceButton->setEnabled(false);
    int row = ui->pricingTable->currentRow();
    if (row < 0) { ui->editPriceButton->setEnabled(true); return; }
    Price price;
    price.room_type = ui->pricingTable->item(row, 0)->text();
    price.price = QInputDialog::getDouble(this, "Fiyat", "Fiyat:", ui->pricingTable->item(row, 1)->text().toDouble());
    QString errorMsg;
    if (Database::instance().editPrice(price, errorMsg)) {
        QMessageBox::information(this, "Başarılı", "Fiyat güncellendi.");
        populatePricingTable();
    } else {
        QMessageBox::warning(this, "Hata", errorMsg);
    }
    ui->editPriceButton->setEnabled(true);
}

void AdminDashboard::on_deletePriceButton_clicked() {
    ui->deletePriceButton->setEnabled(false);
    int row = ui->pricingTable->currentRow();
    if (row < 0) { ui->deletePriceButton->setEnabled(true); return; }
    QString room_type = ui->pricingTable->item(row, 0)->text();
    if (QMessageBox::question(this, "Sil", "Fiyat silinsin mi?") == QMessageBox::Yes) {
        QString errorMsg;
        if (Database::instance().deletePrice(room_type, errorMsg)) {
            QMessageBox::information(this, "Başarılı", "Fiyat silindi.");
            populatePricingTable();
        } else {
            QMessageBox::warning(this, "Hata", errorMsg);
        }
    }
    ui->deletePriceButton->setEnabled(true);
} 