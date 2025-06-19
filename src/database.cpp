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
               "email TEXT"
               ")");
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
    // Fiyatlandırma tablosu
    query.exec("CREATE TABLE IF NOT EXISTS prices ("
               "room_type TEXT PRIMARY KEY,"
               "price REAL"
               ")");
    // Kaç kullanıcı var?
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next()) {
        qDebug() << "Yüklenen kullanıcı sayısı (SQL):" << query.value(0).toInt();
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

bool Database::addReservation(const QString& username, int room_no, const QString& checkin, const QString& checkout, QString& errorMsg) {
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
    QSqlQuery insert;
    insert.prepare("INSERT INTO reservations (username, room_no, checkin, checkout, status, created_at) VALUES (?, ?, ?, ?, 'pending', ?)");
    insert.addBindValue(username);
    insert.addBindValue(room_no);
    insert.addBindValue(checkin);
    insert.addBindValue(checkout);
    insert.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
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
    query.prepare("SELECT id, username, room_no, checkin, checkout, status, created_at FROM reservations WHERE username = ? ORDER BY checkin DESC");
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
            list.append(r);
        }
    }
    return list;
}

QList<Reservation> Database::getAllReservations() {
    QList<Reservation> list;
    QSqlQuery query("SELECT id, username, room_no, checkin, checkout, status, created_at FROM reservations ORDER BY checkin DESC");
    while (query.next()) {
        Reservation r;
        r.id = query.value(0).toInt();
        r.username = query.value(1).toString();
        r.room_no = query.value(2).toInt();
        r.checkin = query.value(3).toString();
        r.checkout = query.value(4).toString();
        r.status = query.value(5).toString();
        r.created_at = query.value(6).toString();
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
    query.prepare("SELECT username, password, role, name, surname, email FROM users WHERE username = ? AND password = ?");
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
        return user;
    }
    return User();
}

bool Database::registerUser(const QString& name, const QString& surname, const QString& username, const QString& email, const QString& password, QString& errorMsg) {
    QSqlQuery check;
    check.prepare("SELECT COUNT(*) FROM users WHERE username = ?");
    check.addBindValue(username);
    if (check.exec() && check.next() && check.value(0).toInt() > 0) {
        errorMsg = "Kullanıcı adı zaten mevcut.";
        return false;
    }
    QSqlQuery insert;
    insert.prepare("INSERT INTO users (username, password, role, name, surname, email) VALUES (?, ?, ?, ?, ?, ?)");
    insert.addBindValue(username);
    insert.addBindValue(password);
    insert.addBindValue("user");
    insert.addBindValue(name);
    insert.addBindValue(surname);
    insert.addBindValue(email);
    if (!insert.exec()) {
        errorMsg = "Kayıt başarısız: " + insert.lastError().text();
        return false;
    }
    return true;
}

User Database::getUser(const QString& username) {
    QSqlQuery query;
    query.prepare("SELECT username, password, role, name, surname, email FROM users WHERE username = ?");
    query.addBindValue(username);
    if (query.exec() && query.next()) {
        User user;
        user.username = query.value(0).toString();
        user.password = query.value(1).toString();
        user.role = query.value(2).toString();
        user.name = query.value(3).toString();
        user.surname = query.value(4).toString();
        user.email = query.value(5).toString();
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
    insert.prepare("INSERT INTO users (username, password, role, name, surname, email) VALUES (?, ?, ?, ?, ?, ?)");
    insert.addBindValue(user.username);
    insert.addBindValue(user.password);
    insert.addBindValue(user.role);
    insert.addBindValue(user.name);
    insert.addBindValue(user.surname);
    insert.addBindValue(user.email);
    if (!insert.exec()) {
        errorMsg = "Kullanıcı eklenemedi: " + insert.lastError().text();
        return false;
    }
    return true;
}

bool Database::editUser(const User& user, QString& errorMsg) {
    QSqlQuery update;
    update.prepare("UPDATE users SET password=?, role=?, name=?, surname=?, email=? WHERE username=?");
    update.addBindValue(user.password);
    update.addBindValue(user.role);
    update.addBindValue(user.name);
    update.addBindValue(user.surname);
    update.addBindValue(user.email);
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