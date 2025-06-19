// admindashboard.h
#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QWidget>

namespace Ui { class AdminDashboard; }
class MainWindow;

class AdminDashboard : public QWidget
{
    Q_OBJECT
public:
    explicit AdminDashboard(QWidget *parent = nullptr);
    ~AdminDashboard();

private slots:
    void on_logoutButton_clicked();

private:
    Ui::AdminDashboard *ui;
    MainWindow* mainWindow;
};

#endif // ADMINDASHBOARD_H 