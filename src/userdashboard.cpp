// userdashboard.cpp 
#include "userdashboard.h"
#include "ui_userdashboard.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QDate>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>

UserDashboard::UserDashboard(QWidget *parent)
    : QWidget(parent), ui(new Ui::UserDashboard), mainWindow(nullptr)
{
    ui->setupUi(this);
    connect(ui->logoutButton, &QPushButton::clicked, this, &UserDashboard::on_logoutButton_clicked);
    connect(ui->createReservationButton, &QPushButton::clicked, this, &UserDashboard::on_createReservationButton_clicked);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &UserDashboard::on_tabWidget_currentChanged);
    connect(ui->roomTypeComboBox, &QComboBox::currentTextChanged, this, &UserDashboard::on_roomTypeOrDateChanged);
    connect(ui->checkInDateEdit, &QDateEdit::dateChanged, this, &UserDashboard::on_roomTypeOrDateChanged);
    connect(ui->checkOutDateEdit, &QDateEdit::dateChanged, this, &UserDashboard::on_roomTypeOrDateChanged);
}

UserDashboard::~UserDashboard() { delete ui; }

void UserDashboard::setUser(const User& user)
{
    currentUser = user;
    ui->welcomeLabel->setText("Hoş Geldiniz, " + user.username);
    populateReservationsTable();
    populateRoomTypeComboBox();
    clearNewReservationForm();
}

void UserDashboard::populateReservationsTable()
{
    QList<Reservation> reservations = Database::instance().getUserReservations(currentUser.username);
    ui->reservationsTable->setRowCount(reservations.size());
    ui->reservationsTable->setColumnCount(6);
    QStringList headers = {"Oda No", "Oda Tipi", "Giriş Tarihi", "Çıkış Tarihi", "Durum", "Oda Fiyatı"};
    ui->reservationsTable->setHorizontalHeaderLabels(headers);
    for (int i = 0; i < reservations.size(); ++i) {
        const Reservation& r = reservations[i];
        Room room;
        QList<Room> allRooms = Database::instance().getAllRooms();
        for (const Room& rm : allRooms) {
            if (rm.room_no == r.room_no) { room = rm; break; }
        }
        ui->reservationsTable->setItem(i, 0, new QTableWidgetItem(QString::number(r.room_no)));
        ui->reservationsTable->setItem(i, 1, new QTableWidgetItem(room.type));
        ui->reservationsTable->setItem(i, 2, new QTableWidgetItem(r.checkin));
        ui->reservationsTable->setItem(i, 3, new QTableWidgetItem(r.checkout));
        ui->reservationsTable->setItem(i, 4, new QTableWidgetItem(r.status));
        ui->reservationsTable->setItem(i, 5, new QTableWidgetItem("-")); // Fiyat entegrasyonu ileride
    }
    ui->reservationsTable->resizeColumnsToContents();
}

void UserDashboard::populateRoomTypeComboBox()
{
    ui->roomTypeComboBox->clear();
    QList<Room> rooms = Database::instance().getAllRooms();
    QSet<QString> types;
    for (const Room& r : rooms) types.insert(r.type);
    for (const QString& t : types) ui->roomTypeComboBox->addItem(t);
    populateAvailableRoomNumbers();
}

void UserDashboard::populateAvailableRoomNumbers()
{
    ui->roomNumberComboBox->clear();
    QString type = ui->roomTypeComboBox->currentText();
    QDate checkin = ui->checkInDateEdit->date();
    QDate checkout = ui->checkOutDateEdit->date();
    QList<Room> availableRooms = Database::instance().getAvailableRooms(checkin.toString("yyyy-MM-dd"), checkout.toString("yyyy-MM-dd"));
    for (const Room& r : availableRooms) {
        if (r.type == type) {
            ui->roomNumberComboBox->addItem(QString::number(r.room_no));
        }
    }
}

void UserDashboard::on_roomTypeOrDateChanged()
{
    populateAvailableRoomNumbers();
}

void UserDashboard::updateAvailableRooms()
{
    // Optionally, show available rooms for selected type and dates (future extension)
}

void UserDashboard::clearNewReservationForm()
{
    ui->roomTypeComboBox->setCurrentIndex(0);
    ui->checkInDateEdit->setDate(QDate::currentDate());
    ui->checkOutDateEdit->setDate(QDate::currentDate().addDays(1));
    ui->guestCountSpinBox->setValue(1);
    populateAvailableRoomNumbers();
}

void UserDashboard::on_createReservationButton_clicked()
{
    ui->createReservationButton->setEnabled(false); // Prevent double click
    QString type = ui->roomTypeComboBox->currentText();
    QDate checkin = ui->checkInDateEdit->date();
    QDate checkout = ui->checkOutDateEdit->date();
    QString roomNoStr = ui->roomNumberComboBox->currentText();
    if (type.isEmpty() || roomNoStr.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Oda tipi ve oda numarası seçiniz.");
        ui->createReservationButton->setEnabled(true);
        return;
    }
    if (!checkin.isValid() || !checkout.isValid() || checkin < QDate::currentDate() || checkout <= checkin) {
        QMessageBox::warning(this, "Hata", "Geçersiz tarih aralığı.");
        ui->createReservationButton->setEnabled(true);
        return;
    }
    int selectedRoomNo = roomNoStr.toInt();
    QString errorMsg;
    if (Database::instance().addReservation(currentUser.username, selectedRoomNo, checkin.toString("yyyy-MM-dd"), checkout.toString("yyyy-MM-dd"), errorMsg)) {
        QMessageBox::information(this, "Başarılı", "Rezervasyon talebiniz alındı. Onay bekliyor.");
        populateReservationsTable();
        clearNewReservationForm();
    } else {
        QMessageBox::warning(this, "Hata", errorMsg);
    }
    ui->createReservationButton->setEnabled(true);
}

void UserDashboard::on_tabWidget_currentChanged(int index)
{
    // 1: Rezervasyonlarım, 2: Yeni Rezervasyon
    if (index == 1) populateReservationsTable();
    else if (index == 2) clearNewReservationForm();
}

void UserDashboard::on_logoutButton_clicked()
{
    emit logoutClicked();
} 