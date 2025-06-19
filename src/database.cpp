// database.cpp 
#include "database.h"
#include <QFile>
#include <QTextStream>

Database::Database() {
    loadUsers();
}

Database& Database::instance() {
    static Database db;
    return db;
}

void Database::loadUsers() {
    users.clear();
    QFile file("data/users.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) continue;
            QStringList parts = line.split(';');
            if (parts.size() >= 3) {
                User user;
                user.username = parts[0];
                user.password = parts[1];
                user.role = parts[2];
                user.name = "";
                user.surname = "";
                user.email = "";
                users[user.username] = user;
            }
        }
        file.close();
    }
}

bool Database::authenticateUser(const QString& username, const QString& password, QString& role) {
    if (users.contains(username) && users[username].password == password) {
        role = users[username].role;
        return true;
    }
    return false;
}

bool Database::registerUser(const QString& name, const QString& surname, const QString& username, const QString& email, const QString& password, QString& errorMsg) {
    if (users.contains(username)) {
        errorMsg = "Kullanıcı adı zaten mevcut.";
        return false;
    }
    User user;
    user.name = name;
    user.surname = surname;
    user.username = username;
    user.email = email;
    user.password = password;
    user.role = "user";
    users[username] = user;
    // Dosyaya ekle
    QFile file("data/users.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << username << ";" << password << ";user\n";
        file.close();
    }
    return true;
}

User Database::getUser(const QString& username) {
    return users.value(username);
} 