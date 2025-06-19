// receptionistdashboard.h
#ifndef RECEPTIONISTDASHBOARD_H
#define RECEPTIONISTDASHBOARD_H

#include <QWidget>

namespace Ui { class ReceptionistDashboard; }
class MainWindow;

class ReceptionistDashboard : public QWidget
{
    Q_OBJECT
public:
    explicit ReceptionistDashboard(QWidget *parent = nullptr);
    ~ReceptionistDashboard();

private slots:
    void on_logoutButton_clicked();

private:
    Ui::ReceptionistDashboard *ui;
    MainWindow* mainWindow;
};

#endif // RECEPTIONISTDASHBOARD_H 