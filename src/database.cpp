// database.cpp 
#include "database.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QCoreApplication>
#include <QDate>

Database::Database() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/hotel.db");
    if (!db.open()) {
        qDebug() << "Veritabanı açılamadı:" << db.lastError().text();
    }
    ensureTablesAndDefaults();
    ensureRoomTableAndDefaults();
}

Database& Database::instance() {
    static Database db;
    return db;
}

void Database::ensureTablesAndDefaults() {
    QSqlQuery query;
    // Kullanıcılar tablosu yoksa oluştur
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "username TEXT PRIMARY KEY,"
               "password TEXT NOT NULL,"
               "role TEXT NOT NULL,"
               "name TEXT,"
               "surname TEXT,"
               "email TEXT,"
               "phone TEXT"
               ")");
    // Migration: phone alanı ekle
    query.exec("ALTER TABLE users ADD COLUMN phone TEXT");
    // Eğer hiç kullanıcı yoksa, varsayılanları ekle
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next() && query.value(0).toInt() == 0) {
        QSqlQuery insert;
        insert.prepare("INSERT INTO users (username, password, role, name, surname, email) VALUES (?, ?, ?, ?, ?, ?)");
        insert.addBindValue("admin"); insert.addBindValue("adminpass"); insert.addBindValue("admin"); insert.addBindValue("Admin"); insert.addBindValue("Yönetici"); insert.addBindValue("admin@hotel.com"); insert.exec();
        insert.addBindValue("guest"); insert.addBindValue("guestpass"); insert.addBindValue("user"); insert.addBindValue("Guest"); insert.addBindValue("Kullanıcı"); insert.addBindValue("guest@hotel.com"); insert.exec();
        insert.addBindValue("recep"); insert.addBindValue("receppass"); insert.addBindValue("receptionist"); insert.addBindValue("Recep"); insert.addBindValue("Resepsiyonist"); insert.addBindValue("recep@hotel.com"); insert.exec();
        qDebug() << "Varsayılan kullanıcılar eklendi.";
    }
    // Rezervasyon tablosu
    query.exec("CREATE TABLE IF NOT EXISTS reservations ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT,"
               "room_no INTEGER,"
               "checkin TEXT,"
               "checkout TEXT,"
               "status TEXT,"
               "created_at TEXT"
               ")");
    // Migration: adult_count ve child_count ekle
    query.exec("ALTER TABLE reservations ADD COLUMN adult_count INTEGER");
    query.exec("ALTER TABLE reservations ADD COLUMN child_count INTEGER");
    // Fiyatlandırma tablosu
    query.exec("CREATE TABLE IF NOT EXISTS prices ("
               "room_type TEXT PRIMARY KEY,"
               "price REAL"
               ")");
    // Migration: price alanı ekle
    query.exec("ALTER TABLE reservations ADD COLUMN price REAL");
    // Migration: kredi kartı ve ödeme durumu alanları
    query.exec("ALTER TABLE reservations ADD COLUMN card_number TEXT");
    query.exec("ALTER TABLE reservations ADD COLUMN card_expiry TEXT");
    query.exec("ALTER TABLE reservations ADD COLUMN card_cvv TEXT");
    query.exec("ALTER TABLE reservations ADD COLUMN payment_status TEXT");
    // Kaç kullanıcı var?
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next()) {
        qDebug() << "Yüklenen kullanıcı sayısı (SQL):" << query.value(0).toInt();
    }
    // Settings tablosu
    query.exec("CREATE TABLE IF NOT EXISTS settings (hotel_name TEXT, hotel_address TEXT, hotel_contact TEXT)");
    query.exec("SELECT COUNT(*) FROM settings");
    if (query.next() && query.value(0).toInt() == 0) {
        QSqlQuery insert;
        insert.prepare("INSERT INTO settings (hotel_name, hotel_address, hotel_contact) VALUES (?, ?, ?)");
        insert.addBindValue("Grand Hotel");
        insert.addBindValue("Adres girilmedi");
        insert.addBindValue("0123 456 78 90");
        insert.exec();
    }
}

void Database::ensureRoomTableAndDefaults() {
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS rooms ("
               "room_no INTEGER PRIMARY KEY,"
               "floor INTEGER,"
               "type TEXT,"
               "status TEXT"
               ")");
    // Eğer hiç oda yoksa, 3 kat x 20 oda ekle
    query.exec("SELECT COUNT(*) FROM rooms");
    if (query.next() && query.value(0).toInt() == 0) {
        QSqlQuery insert;
        for (int floor = 1; floor <= 3; ++floor) {
            for (int i = 1; i <= 20; ++i) {
                int room_no = floor * 100 + i;
                QString type;
                if (floor == 1) type = "standart";
                else if (floor == 2) type = "suit";
                else if (floor == 3) type = "deluxe";
                insert.prepare("INSERT INTO rooms (room_no, floor, type, status) VALUES (?, ?, ?, ?)");
                insert.addBindValue(room_no);
                insert.addBindValue(floor);
                insert.addBindValue(type);
                insert.addBindValue("available");
                insert.exec();
            }
        }
        qDebug() << "Varsayılan odalar eklendi.";
    }
}

QList<Room> Database::getAllRooms() {
    QList<Room> rooms;
    QSqlQuery query("SELECT room_no, floor, type, status FROM rooms ORDER BY room_no");
    while (query.next()) {
        Room r;
        r.room_no = query.value(0).toInt();
        r.floor = query.value(1).toInt();
        r.type = query.value(2).toString();
        r.status = query.value(3).toString();
        rooms.append(r);
    }
    return rooms;
}

QList<Room> Database::getAvailableRooms(const QString& checkin, const QString& checkout) {
    QList<Room> available;
    QSqlQuery query("SELECT room_no, floor, type, status FROM rooms WHERE status = 'available' ORDER BY room_no");
    while (query.next()) {
        int room_no = query.value(0).toInt();
        // Oda bu tarihlerde müsait mi?
        if (isRoomAvailable(room_no, checkin, checkout)) {
            Room r;
            r.room_no = room_no;
            r.floor = query.value(1).toInt();
            r.type = query.value(2).toString();
            r.status = query.value(3).toString();
            available.append(r);
        }
    }
    return available;
}

bool Database::isRoomAvailable(int room_no, const QString& checkin, const QString& checkout) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM reservations WHERE room_no = ? AND status IN ('pending','approved','checked_in') AND NOT (checkout <= ? OR checkin >= ?)");
    query.addBindValue(room_no);
    query.addBindValue(checkin); // checkout <= checkin (yani bitiş tarihi yeni girişten önceyse çakışmaz)
    query.addBindValue(checkout); // checkin >= checkout (yani başlangıç tarihi yeni çıkıştan sonra ise çakışmaz)
    if (query.exec() && query.next()) {
        return query.value(0).toInt() == 0;
    }
    return false;
}

bool Database::addReservation(const QString& username, int room_no, const QString& checkin, const QString& checkout, int adult_count, int child_count, double price, const QString& card_number, const QString& card_expiry, const QString& card_cvv, QString& errorMsg) {
    // Tarih kontrolü
    QDate today = QDate::currentDate();
    QDate d_checkin = QDate::fromString(checkin, "yyyy-MM-dd");
    QDate d_checkout = QDate::fromString(checkout, "yyyy-MM-dd");
    if (!d_checkin.isValid() || !d_checkout.isValid() || d_checkin < today || d_checkout <= d_checkin) {
        errorMsg = "Geçersiz tarih aralığı.";
        return false;
    }
    if (!isRoomAvailable(room_no, checkin, checkout)) {
        errorMsg = "Oda bu tarihlerde müsait değil.";
        return false;
    }
    // Oda tipine göre kişi kısıtı
    QSqlQuery roomQuery;
    roomQuery.prepare("SELECT type FROM rooms WHERE room_no = ?");
    roomQuery.addBindValue(room_no);
    if (!roomQuery.exec() || !roomQuery.next()) {
        errorMsg = "Oda tipi bulunamadı.";
        return false;
    }
    QString type = roomQuery.value(0).toString();
    int max_adult = 2, max_child = 1;
    if (type == "suit") { max_adult = 3; max_child = 2; }
    else if (type == "deluxe") { max_adult = 4; max_child = 2; }
    if (adult_count < 1 || adult_count > max_adult || child_count < 0 || child_count > max_child) {
        errorMsg = QString("Bu oda için maksimum %1 yetişkin ve %2 çocuk kalabilir.").arg(max_adult).arg(max_child);
        return false;
    }
    QSqlQuery insert;
    insert.prepare("INSERT INTO reservations (username, room_no, checkin, checkout, status, created_at, adult_count, child_count, price, card_number, card_expiry, card_cvv, payment_status) VALUES (?, ?, ?, ?, 'pending', ?, ?, ?, ?, ?, ?, ?, 'paid')");
    insert.addBindValue(username);
    insert.addBindValue(room_no);
    insert.addBindValue(checkin);
    insert.addBindValue(checkout);
    insert.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
    insert.addBindValue(adult_count);
    insert.addBindValue(child_count);
    insert.addBindValue(price);
    insert.addBindValue(card_number);
    insert.addBindValue(card_expiry);
    insert.addBindValue(card_cvv);
    if (!insert.exec()) {
        errorMsg = "Rezervasyon eklenemedi: " + insert.lastError().text();
        return false;
    }
    // Oda durumunu 'reserved' yap
    QSqlQuery updateRoom;
    updateRoom.prepare("UPDATE rooms SET status = 'reserved' WHERE room_no = ?");
    updateRoom.addBindValue(room_no);
    updateRoom.exec();
    return true;
}

QList<Reservation> Database::getUserReservations(const QString& username) {
    QList<Reservation> list;
    QSqlQuery query;
    query.prepare("SELECT id, username, room_no, checkin, checkout, status, created_at, adult_count, child_count, price, card_number, card_expiry, card_cvv, payment_status FROM reservations WHERE username = ? ORDER BY checkin DESC");
    query.addBindValue(username);
    if (query.exec()) {
        while (query.next()) {
            Reservation r;
            r.id = query.value(0).toInt();
            r.username = query.value(1).toString();
            r.room_no = query.value(2).toInt();
            r.checkin = query.value(3).toString();
            r.checkout = query.value(4).toString();
            r.status = query.value(5).toString();
            r.created_at = query.value(6).toString();
            r.adult_count = query.value(7).toInt();
            r.child_count = query.value(8).toInt();
            r.price = query.value(9).toDouble();
            r.card_number = query.value(10).toString();
            r.card_expiry = query.value(11).toString();
            r.card_cvv = query.value(12).toString();
            r.payment_status = query.value(13).toString();
            list.append(r);
        }
    }
    return list;
}

QList<Reservation> Database::getAllReservations() {
    QList<Reservation> list;
    QSqlQuery query("SELECT id, username, room_no, checkin, checkout, status, created_at, adult_count, child_count, price, card_number, card_expiry, card_cvv, payment_status FROM reservations ORDER BY checkin DESC");
    while (query.next()) {
        Reservation r;
        r.id = query.value(0).toInt();
        r.username = query.value(1).toString();
        r.room_no = query.value(2).toInt();
        r.checkin = query.value(3).toString();
        r.checkout = query.value(4).toString();
        r.status = query.value(5).toString();
        r.created_at = query.value(6).toString();
        r.adult_count = query.value(7).toInt();
        r.child_count = query.value(8).toInt();
        r.price = query.value(9).toDouble();
        r.card_number = query.value(10).toString();
        r.card_expiry = query.value(11).toString();
        r.card_cvv = query.value(12).toString();
        r.payment_status = query.value(13).toString();
        list.append(r);
    }
    return list;
}

bool Database::updateReservationStatus(int reservation_id, const QString& new_status) {
    QSqlQuery query;
    query.prepare("UPDATE reservations SET status = ? WHERE id = ?");
    query.addBindValue(new_status);
    query.addBindValue(reservation_id);
    bool ok = query.exec();
    if (!ok) return false;
    // Oda durumunu güncelle
    QSqlQuery getRoom;
    getRoom.prepare("SELECT room_no FROM reservations WHERE id = ?");
    getRoom.addBindValue(reservation_id);
    if (getRoom.exec() && getRoom.next()) {
        int room_no = getRoom.value(0).toInt();
        QSqlQuery updateRoom;
        if (new_status == "approved" || new_status == "pending") {
            updateRoom.prepare("UPDATE rooms SET status = 'reserved' WHERE room_no = ?");
        } else if (new_status == "checked_in") {
            updateRoom.prepare("UPDATE rooms SET status = 'occupied' WHERE room_no = ?");
        } else if (new_status == "checked_out" || new_status == "cancelled") {
            updateRoom.prepare("UPDATE rooms SET status = 'available' WHERE room_no = ?");
        }
        updateRoom.addBindValue(room_no);
        updateRoom.exec();
    }
    return true;
}

User Database::authenticateUser(const QString& username, const QString& password) {
    QSqlQuery query;
    query.prepare("SELECT username, password, role, name, surname, email, phone FROM users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(password);
    if (query.exec() && query.next()) {
        User user;
        user.username = query.value(0).toString();
        user.password = query.value(1).toString();
        user.role = query.value(2).toString();
        user.name = query.value(3).toString();
        user.surname = query.value(4).toString();
        user.email = query.value(5).toString();
        user.phone = query.value(6).toString();
        return user;
    }
    return User();
}

bool Database::registerUser(const QString& name, const QString& surname, const QString& username, const QString& email, const QString& phone, const QString& password, QString& errorMsg) {
    QSqlQuery check;
    check.prepare("SELECT COUNT(*) FROM users WHERE username = ?");
    check.addBindValue(username);
    if (check.exec() && check.next() && check.value(0).toInt() > 0) {
        errorMsg = "Kullanıcı adı zaten mevcut.";
        return false;
    }
    QSqlQuery insert;
    insert.prepare("INSERT INTO users (username, password, role, name, surname, email, phone) VALUES (?, ?, ?, ?, ?, ?, ?)");
    insert.addBindValue(username);
    insert.addBindValue(password);
    insert.addBindValue("user");
    insert.addBindValue(name);
    insert.addBindValue(surname);
    insert.addBindValue(email);
    insert.addBindValue(phone);
    if (!insert.exec()) {
        errorMsg = "Kayıt başarısız: " + insert.lastError().text();
        return false;
    }
    return true;
}

User Database::getUser(const QString& username) {
    QSqlQuery query;
    query.prepare("SELECT username, password, role, name, surname, email, phone FROM users WHERE username = ?");
    query.addBindValue(username);
    if (query.exec() && query.next()) {
        User user;
        user.username = query.value(0).toString();
        user.password = query.value(1).toString();
        user.role = query.value(2).toString();
        user.name = query.value(3).toString();
        user.surname = query.value(4).toString();
        user.email = query.value(5).toString();
        user.phone = query.value(6).toString();
        return user;
    }
    return User();
}

bool Database::addUser(const User& user, QString& errorMsg) {
    QSqlQuery check;
    check.prepare("SELECT COUNT(*) FROM users WHERE username = ?");
    check.addBindValue(user.username);
    if (check.exec() && check.next() && check.value(0).toInt() > 0) {
        errorMsg = "Kullanıcı adı zaten mevcut.";
        return false;
    }
    QSqlQuery insert;
    insert.prepare("INSERT INTO users (username, password, role, name, surname, email, phone) VALUES (?, ?, ?, ?, ?, ?, ?)");
    insert.addBindValue(user.username);
    insert.addBindValue(user.password);
    insert.addBindValue(user.role);
    insert.addBindValue(user.name);
    insert.addBindValue(user.surname);
    insert.addBindValue(user.email);
    insert.addBindValue(user.phone);
    if (!insert.exec()) {
        errorMsg = "Kullanıcı eklenemedi: " + insert.lastError().text();
        return false;
    }
    return true;
}

bool Database::editUser(const User& user, QString& errorMsg) {
    QSqlQuery update;
    update.prepare("UPDATE users SET password=?, role=?, name=?, surname=?, email=?, phone=? WHERE username=?");
    update.addBindValue(user.password);
    update.addBindValue(user.role);
    update.addBindValue(user.name);
    update.addBindValue(user.surname);
    update.addBindValue(user.email);
    update.addBindValue(user.phone);
    update.addBindValue(user.username);
    if (!update.exec()) {
        errorMsg = "Kullanıcı güncellenemedi: " + update.lastError().text();
        return false;
    }
    return true;
}

bool Database::deleteUser(const QString& username, QString& errorMsg) {
    QSqlQuery del;
    del.prepare("DELETE FROM users WHERE username=?");
    del.addBindValue(username);
    if (!del.exec()) {
        errorMsg = "Kullanıcı silinemedi: " + del.lastError().text();
        return false;
    }
    return true;
}

bool Database::addRoom(const Room& room, QString& errorMsg) {
    QSqlQuery check;
    check.prepare("SELECT COUNT(*) FROM rooms WHERE room_no = ?");
    check.addBindValue(room.room_no);
    if (check.exec() && check.next() && check.value(0).toInt() > 0) {
        errorMsg = "Oda numarası zaten mevcut.";
        return false;
    }
    QSqlQuery insert;
    insert.prepare("INSERT INTO rooms (room_no, floor, type, status) VALUES (?, ?, ?, ?)");
    insert.addBindValue(room.room_no);
    insert.addBindValue(room.floor);
    insert.addBindValue(room.type);
    insert.addBindValue(room.status);
    if (!insert.exec()) {
        errorMsg = "Oda eklenemedi: " + insert.lastError().text();
        return false;
    }
    return true;
}

bool Database::editRoom(const Room& room, QString& errorMsg) {
    QSqlQuery update;
    update.prepare("UPDATE rooms SET floor=?, type=?, status=? WHERE room_no=?");
    update.addBindValue(room.floor);
    update.addBindValue(room.type);
    update.addBindValue(room.status);
    update.addBindValue(room.room_no);
    if (!update.exec()) {
        errorMsg = "Oda güncellenemedi: " + update.lastError().text();
        return false;
    }
    return true;
}

bool Database::deleteRoom(int room_no, QString& errorMsg) {
    QSqlQuery del;
    del.prepare("DELETE FROM rooms WHERE room_no=?");
    del.addBindValue(room_no);
    if (!del.exec()) {
        errorMsg = "Oda silinemedi: " + del.lastError().text();
        return false;
    }
    return true;
}

bool Database::addPrice(const Price& price, QString& errorMsg) {
    QSqlQuery check;
    check.prepare("SELECT COUNT(*) FROM prices WHERE room_type = ?");
    check.addBindValue(price.room_type);
    if (check.exec() && check.next() && check.value(0).toInt() > 0) {
        errorMsg = "Bu oda tipi için fiyat zaten mevcut.";
        return false;
    }
    QSqlQuery insert;
    insert.prepare("INSERT INTO prices (room_type, price) VALUES (?, ?)");
    insert.addBindValue(price.room_type);
    insert.addBindValue(price.price);
    if (!insert.exec()) {
        errorMsg = "Fiyat eklenemedi: " + insert.lastError().text();
        return false;
    }
    return true;
}

bool Database::editPrice(const Price& price, QString& errorMsg) {
    QSqlQuery update;
    update.prepare("UPDATE prices SET price=? WHERE room_type=?");
    update.addBindValue(price.price);
    update.addBindValue(price.room_type);
    if (!update.exec()) {
        errorMsg = "Fiyat güncellenemedi: " + update.lastError().text();
        return false;
    }
    return true;
}

bool Database::deletePrice(const QString& room_type, QString& errorMsg) {
    QSqlQuery del;
    del.prepare("DELETE FROM prices WHERE room_type=?");
    del.addBindValue(room_type);
    if (!del.exec()) {
        errorMsg = "Fiyat silinemedi: " + del.lastError().text();
        return false;
    }
    return true;
}

QList<Price> Database::getAllPrices() {
    QList<Price> prices;
    QSqlQuery query("SELECT room_type, price FROM prices");
    while (query.next()) {
        Price p;
        p.room_type = query.value(0).toString();
        p.price = query.value(1).toDouble();
        prices.append(p);
    }
    return prices;
}

double Database::getPriceForRoomType(const QString& room_type) {
    QSqlQuery query;
    query.prepare("SELECT price FROM prices WHERE room_type = ?");
    query.addBindValue(room_type);
    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

double Database::getDefaultPriceForRoomType(const QString& room_type) {
    if (room_type == "standart") return 1750;
    if (room_type == "suit") return 2350;
    if (room_type == "deluxe") return 4999;
    return 0.0;
}

Settings Database::getSettings() {
    Settings s;
    QSqlQuery query("SELECT hotel_name, hotel_address, hotel_contact FROM settings LIMIT 1");
    if (query.exec() && query.next()) {
        s.hotel_name = query.value(0).toString();
        s.hotel_address = query.value(1).toString();
        s.hotel_contact = query.value(2).toString();
    }
    return s;
}

bool Database::updateSettings(const Settings& settings) {
    QSqlQuery query;
    query.prepare("UPDATE settings SET hotel_name=?, hotel_address=?, hotel_contact=?");
    query.addBindValue(settings.hotel_name);
    query.addBindValue(settings.hotel_address);
    query.addBindValue(settings.hotel_contact);
    return query.exec();
} 