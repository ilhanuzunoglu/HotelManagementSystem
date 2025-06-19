// database.cpp 
#include "database.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QCoreApplication>

Database::Database() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/hotel.db");
    if (!db.open()) {
        qDebug() << "Veritabanı açılamadı:" << db.lastError().text();
    }
    ensureTablesAndDefaults();
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
    // Kaç kullanıcı var?
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next()) {
        qDebug() << "Yüklenen kullanıcı sayısı (SQL):" << query.value(0).toInt();
    }
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