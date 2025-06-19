#include <QApplication>
#include "mainwindow.h"
#include "database.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Database::instance(); // Kullanıcıları yükle
    MainWindow w;
    w.show();
    return a.exec();
} 