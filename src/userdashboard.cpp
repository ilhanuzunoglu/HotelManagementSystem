// userdashboard.cpp 
#include "userdashboard.h"
#include "ui_userdashboard.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QDate>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include <QRegularExpressionValidator>

UserDashboard::UserDashboard(QWidget *parent)
    : QWidget(parent), ui(new Ui::UserDashboard), mainWindow(nullptr)
{
    ui->setupUi(this);
    profilePhoneEdit = new QLineEdit(this);
    QRegularExpression phoneRegex(R"(^[0-9]{10,15}$)");
    profilePhoneEdit->setValidator(new QRegularExpressionValidator(phoneRegex, this));
    ui->profileFormLayout->insertRow(3, tr("Telefon:"), profilePhoneEdit);
    QRegularExpression cardRegex(R"(^[0-9]{16}$)");
    QRegularExpression expiryRegex(R"(^(0[1-9]|1[0-2])\/(\d{2}|\d{4})$)");
    QRegularExpression cvvRegex(R"(^[0-9]{3,4}$)");
    ui->cardNumberEdit->setInputMask("");
    ui->cardNumberEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(R"(^([0-9]{0,4}( [0-9]{0,4}){0,3})?$)"), this));
    ui->cardExpiryEdit->setInputMask("");
    ui->cardExpiryEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(R"(^[0-9]{0,2}/?[0-9]{0,2}$)"), this));
    ui->cardCvvEdit->setInputMask("");
    ui->cardCvvEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(R"(^[0-9]{0,3}$)"), this));
    connect(ui->logoutButton, &QPushButton::clicked, this, &UserDashboard::on_logoutButton_clicked);
    connect(ui->createReservationButton, &QPushButton::clicked, this, &UserDashboard::on_createReservationButton_clicked);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &UserDashboard::on_tabWidget_currentChanged);
    connect(ui->roomTypeComboBox, &QComboBox::currentTextChanged, this, &UserDashboard::on_roomTypeOrDateChanged);
    connect(ui->checkInDateEdit, &QDateEdit::dateChanged, this, &UserDashboard::on_roomTypeOrDateChanged);
    connect(ui->checkOutDateEdit, &QDateEdit::dateChanged, this, &UserDashboard::on_roomTypeOrDateChanged);
    connect(ui->updateProfileButton, &QPushButton::clicked, this, &UserDashboard::on_updateProfileButton_clicked);
    ui->profileUsernameEdit->setReadOnly(true);
}

UserDashboard::~UserDashboard() { delete ui; }

void UserDashboard::setUser(const User& user)
{
    currentUser = user;
    ui->welcomeLabel->setText("Hoş Geldiniz, " + user.username);
    populateReservationsTable();
    populateRoomTypeComboBox();
    clearNewReservationForm();
    // Profil alanlarını doldur
    ui->profileNameEdit->setText(user.name);
    ui->profileSurnameEdit->setText(user.surname);
    ui->profileEmailEdit->setText(user.email);
    ui->profileUsernameEdit->setText(user.username);
    profilePhoneEdit->setText(user.phone);
}

void UserDashboard::populateReservationsTable()
{
    QList<Reservation> reservations = Database::instance().getUserReservations(currentUser.username);
    ui->reservationsTable->setRowCount(reservations.size());
    ui->reservationsTable->setColumnCount(8);
    QStringList headers = {"Oda No", "Oda Tipi", "Giriş Tarihi", "Çıkış Tarihi", "Durum", "Oda Fiyatı", "Yetişkin", "Çocuk"};
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
        ui->reservationsTable->setItem(i, 5, new QTableWidgetItem(QString::number(r.price, 'f', 2)));
        ui->reservationsTable->setItem(i, 6, new QTableWidgetItem(QString::number(r.adult_count)));
        ui->reservationsTable->setItem(i, 7, new QTableWidgetItem(QString::number(r.child_count)));
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
    ui->adultCountSpinBox->setValue(1);
    ui->childCountSpinBox->setValue(0);
    ui->cardNumberEdit->clear();
    ui->cardExpiryEdit->clear();
    ui->cardCvvEdit->clear();
    populateAvailableRoomNumbers();
}

void UserDashboard::on_createReservationButton_clicked()
{
    ui->createReservationButton->setEnabled(false); // Prevent double click
    QString type = ui->roomTypeComboBox->currentText();
    QDate checkin = ui->checkInDateEdit->date();
    QDate checkout = ui->checkOutDateEdit->date();
    QString roomNoStr = ui->roomNumberComboBox->currentText();
    int adultCount = ui->adultCountSpinBox->value();
    int childCount = ui->childCountSpinBox->value();
    double price = Database::instance().getPriceForRoomType(type);
    QString cardNumber = ui->cardNumberEdit->text();
    QString cardExpiry = ui->cardExpiryEdit->text();
    QString cardCvv = ui->cardCvvEdit->text();
    QRegularExpression cardRegex(R"(^[0-9]{4} [0-9]{4} [0-9]{4} [0-9]{4}$)");
    QRegularExpression expiryRegex(R"(^(0[1-9]|1[0-2])\/(\d{2})$)");
    QRegularExpression cvvRegex(R"(^[0-9]{3}$)");
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
    if (cardNumber.isEmpty() || cardExpiry.isEmpty() || cardCvv.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Kredi kartı bilgilerini eksiksiz giriniz.");
        ui->createReservationButton->setEnabled(true);
        return;
    }
    if (!cardRegex.match(cardNumber).hasMatch()) {
        QMessageBox::warning(this, "Hata", "Geçerli bir kart numarası girin (4 gruplu 16 rakam).");
        ui->createReservationButton->setEnabled(true);
        return;
    }
    if (!expiryRegex.match(cardExpiry).hasMatch()) {
        QMessageBox::warning(this, "Hata", "Geçerli bir son kullanma tarihi girin (AA/YY).");
        ui->createReservationButton->setEnabled(true);
        return;
    }
    if (!cvvRegex.match(cardCvv).hasMatch()) {
        QMessageBox::warning(this, "Hata", "Geçerli bir CVV girin (3 rakam).");
        ui->createReservationButton->setEnabled(true);
        return;
    }
    // Son kullanma tarihi geçmiş mi kontrolü
    QStringList parts = cardExpiry.split('/');
    if (parts.size() == 2) {
        int month = parts[0].toInt();
        int year = parts[1].toInt();
        QDate now = QDate::currentDate();
        int currentYear = now.year() % 100;
        int currentMonth = now.month();
        if (year < currentYear || (year == currentYear && month < currentMonth)) {
            QMessageBox::warning(this, "Hata", "Kartın son kullanma tarihi geçmiş.");
            ui->createReservationButton->setEnabled(true);
            return;
        }
    }
    int selectedRoomNo = roomNoStr.toInt();
    QString errorMsg;
    if (Database::instance().addReservation(currentUser.username, selectedRoomNo, checkin.toString("yyyy-MM-dd"), checkout.toString("yyyy-MM-dd"), adultCount, childCount, price, cardNumber, cardExpiry, cardCvv, errorMsg)) {
        QMessageBox::information(this, "Başarılı", "Rezervasyon ve ödeme işleminiz başarıyla tamamlandı.");
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
    if (ui->tabWidget->widget(index)->objectName() == "roomInfoTab") {
        populateRoomInfoTab();
    }
}

void UserDashboard::on_logoutButton_clicked()
{
    emit logoutClicked();
}

void UserDashboard::on_updateProfileButton_clicked()
{
    QString name = ui->profileNameEdit->text();
    QString surname = ui->profileSurnameEdit->text();
    QString email = ui->profileEmailEdit->text();
    QString phone = profilePhoneEdit->text();
    QString username = ui->profileUsernameEdit->text();
    QRegularExpression emailRegex(R"(^[\w\.-]+@[\w\.-]+\.[a-zA-Z]{2,}$)");
    if (name.isEmpty() || surname.isEmpty() || email.isEmpty() || phone.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Tüm alanları doldurun.");
        return;
    }
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Hata", "Geçerli bir e-posta girin.");
        return;
    }
    if (!profilePhoneEdit->hasAcceptableInput()) {
        QMessageBox::warning(this, "Hata", "Geçerli bir telefon numarası girin (10-15 rakam).");
        return;
    }
    User updated = currentUser;
    updated.name = name;
    updated.surname = surname;
    updated.email = email;
    updated.phone = phone;
    QString errorMsg;
    if (Database::instance().editUser(updated, errorMsg)) {
        QMessageBox::information(this, "Başarılı", "Profiliniz güncellendi.");
        setUser(updated);
    } else {
        QMessageBox::warning(this, "Hata", errorMsg);
    }
}

void UserDashboard::populateRoomInfoTab()
{
    QTableWidget* table = ui->roomInfoTable;
    table->setRowCount(3);
    table->setColumnCount(3);
    QStringList types = {"standart", "suit", "deluxe"};
    QStringList descriptions = {
        "Standart: Temel konfor, uygun fiyat.",
        "Suit: Geniş alan, oturma grubu, ekstra konfor.",
        "Deluxe: Lüks, manzara, premium hizmet."
    };
    for (int i = 0; i < 3; ++i) {
        table->setItem(i, 0, new QTableWidgetItem(types[i]));
        table->setItem(i, 1, new QTableWidgetItem(descriptions[i]));
        double price = Database::instance().getPriceForRoomType(types[i]);
        if (price <= 0) {
            price = Database::instance().getDefaultPriceForRoomType(types[i]);
        }
        table->setItem(i, 2, new QTableWidgetItem(QString::number(price, 'f', 2)));
    }
    table->setHorizontalHeaderLabels({"Oda Tipi", "Açıklama", "Fiyat (TL)"});
    table->resizeColumnsToContents();
} 