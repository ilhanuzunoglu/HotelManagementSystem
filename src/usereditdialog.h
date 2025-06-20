#ifndef USEREDITDIALOG_H
#define USEREDITDIALOG_H
#include <QDialog>
#include "database.h"

class QLineEdit;
class QComboBox;
class QPushButton;
class QLabel;

class UserEditDialog : public QDialog {
    Q_OBJECT
public:
    explicit UserEditDialog(QWidget* parent = nullptr);
    void setUser(const User& user);
    User getUser() const;
    void setEditMode(bool edit);
    void setRoleSelectionVisible(bool visible);
    void accept() override;
private:
    QLineEdit* nameEdit;
    QLineEdit* surnameEdit;
    QLineEdit* usernameEdit;
    QLineEdit* emailEdit;
    QLineEdit* phoneEdit;
    QLineEdit* passwordEdit;
    QComboBox* roleCombo;
    QPushButton* okButton;
    QPushButton* cancelButton;
    bool editMode;
    QLabel* userRoleLabel = nullptr;
};
#endif // USEREDITDIALOG_H 