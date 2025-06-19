// database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QList>
#include <QSqlDatabase>

struct User {
    QString name;
    QString surname;
    QString username;
    QString email;
    QString password;
    QString role;
};

struct Room {
    int room_no;
    int floor;
    QString type;
    QString status; // available, occupied, cleaning, etc.
};

struct Reservation {
    int id;
    QString username;
    int room_no;
    QString checkin;
    QString checkout;
    QString status; // pending, approved, checked_in, checked_out, cancelled
    QString created_at;
};

struct Price {
    QString room_type;
    double price;
};

class Database {
public:
    static Database& instance();
    User authenticateUser(const QString& username, const QString& password);
    bool registerUser(const QString& name, const QString& surname, const QString& username, const QString& email, const QString& password, QString& errorMsg);
    User getUser(const QString& username);
    void ensureTablesAndDefaults();
    void ensureRoomTableAndDefaults();
    QList<Room> getAllRooms();
    QList<Room> getAvailableRooms(const QString& checkin, const QString& checkout);
    bool addReservation(const QString& username, int room_no, const QString& checkin, const QString& checkout, QString& errorMsg);
    QList<Reservation> getUserReservations(const QString& username);
    QList<Reservation> getAllReservations();
    bool updateReservationStatus(int reservation_id, const QString& new_status);
    bool isRoomAvailable(int room_no, const QString& checkin, const QString& checkout);
    // User management
    bool addUser(const User& user, QString& errorMsg);
    bool editUser(const User& user, QString& errorMsg);
    bool deleteUser(const QString& username, QString& errorMsg);
    // Room management
    bool addRoom(const Room& room, QString& errorMsg);
    bool editRoom(const Room& room, QString& errorMsg);
    bool deleteRoom(int room_no, QString& errorMsg);
    // Pricing management
    bool addPrice(const Price& price, QString& errorMsg);
    bool editPrice(const Price& price, QString& errorMsg);
    bool deletePrice(const QString& room_type, QString& errorMsg);
    QList<Price> getAllPrices();
private:
    Database();
    QSqlDatabase db;
};

#endif // DATABASE_H 