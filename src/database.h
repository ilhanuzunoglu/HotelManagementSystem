// database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QMap>
#include <QStringList>

struct User {
    QString name;
    QString surname;
    QString username;
    QString email;
    QString password;
    QString role;
};

class Database {
public:
    static Database& instance();
    bool authenticateUser(const QString& username, const QString& password, QString& role);
    bool registerUser(const QString& name, const QString& surname, const QString& username, const QString& email, const QString& password, QString& errorMsg);
    void loadUsers();
    User getUser(const QString& username);
private:
    Database();
    QMap<QString, User> users;
};

#endif // DATABASE_H 