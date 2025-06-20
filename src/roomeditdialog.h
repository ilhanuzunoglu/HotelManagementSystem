#ifndef ROOMEDITDIALOG_H
#define ROOMEDITDIALOG_H
#include <QDialog>
#include "database.h"

class QLineEdit;
class QComboBox;
class QPushButton;

class RoomEditDialog : public QDialog {
    Q_OBJECT
public:
    explicit RoomEditDialog(QWidget* parent = nullptr);
    void setRoom(const Room& room);
    Room getRoom() const;
    void setEditMode(bool edit);
private:
    QLineEdit* roomNoEdit;
    QLineEdit* floorEdit;
    QComboBox* typeCombo;
    QComboBox* statusCombo;
    QPushButton* okButton;
    QPushButton* cancelButton;
    bool editMode;
};
#endif // ROOMEDITDIALOG_H 