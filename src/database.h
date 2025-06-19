// database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QMap>
#include <QStringList>
#include <QSqlDatabase>

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
    User authenticateUser(const QString& username, const QString& password);
    bool registerUser(const QString& name, const QString& surname, const QString& username, const QString& email, const QString& password, QString& errorMsg);
    User getUser(const QString& username);
    void ensureTablesAndDefaults();
private:
    Database();
    QSqlDatabase db;
};

#endif // DATABASE_H 