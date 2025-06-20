#include "usereditdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QLabel>

UserEditDialog::UserEditDialog(QWidget* parent) : QDialog(parent), editMode(false) {
    setWindowTitle("Kullanıcı Düzenle/Ekle");
    QFormLayout* form = new QFormLayout;
    nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("Ad");
    nameEdit->setStyleSheet("background-color: #F5F7FA; color: #85929E; font-style: italic; border: 1px solid #B2BABB; border-radius: 8px; margin-bottom: 8px; padding-left: 8px;");
    surnameEdit = new QLineEdit;
    surnameEdit->setPlaceholderText("Soyad");
    surnameEdit->setStyleSheet("background-color: #F5F7FA; color: #85929E; font-style: italic; border: 1px solid #B2BABB; border-radius: 8px; margin-bottom: 8px; padding-left: 8px;");
    usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("Kullanıcı Adı (en az 4 karakter)");
    usernameEdit->setStyleSheet("background-color: #F5F7FA; color: #85929E; font-style: italic; border: 1px solid #B2BABB; border-radius: 8px; margin-bottom: 8px; padding-left: 8px;");
    emailEdit = new QLineEdit;
    emailEdit->setPlaceholderText("ornek@mail.com");
    emailEdit->setStyleSheet("");
    phoneEdit = new QLineEdit;
    phoneEdit->setPlaceholderText("(5xx) xxx xx xx");
    phoneEdit->setStyleSheet("");
    passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("Şifre (en az 6 karakter)");
    passwordEdit->setStyleSheet("background-color: #F5F7FA; color: #85929E; font-style: italic; border: 1px solid #B2BABB; border-radius: 8px; margin-bottom: 8px; padding-left: 8px;");
    passwordEdit->setEchoMode(QLineEdit::Password);
    roleCombo = new QComboBox;
    roleCombo->addItems({"user", "receptionist", "admin"});
    form->addRow("Ad:", nameEdit);
    form->addRow("Soyad:", surnameEdit);
    form->addRow("Kullanıcı Adı:", usernameEdit);
    form->addRow("E-posta:", emailEdit);
    form->addRow("Telefon:", phoneEdit);
    form->addRow("Şifre:", passwordEdit);
    form->addRow("Rol:", roleCombo);
    // Regex validator for email
    QRegularExpression emailRegex(R"(^[\w\.-]+@[\w\.-]+\.[a-zA-Z]{2,}$)");
    emailEdit->setValidator(new QRegularExpressionValidator(emailRegex, this));
    // Regex validator for phone (10-15 digit, only numbers)
    QRegularExpression phoneRegex(R"(^[0-9]{10,15}$)");
    phoneEdit->setValidator(new QRegularExpressionValidator(phoneRegex, this));
    okButton = new QPushButton("Kaydet");
    cancelButton = new QPushButton("İptal");
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(okButton);
    btnLayout->addWidget(cancelButton);
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(form);
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);
    connect(okButton, &QPushButton::clicked, this, &UserEditDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void UserEditDialog::setUser(const User& user) {
    nameEdit->setText(user.name);
    surnameEdit->setText(user.surname);
    usernameEdit->setText(user.username);
    emailEdit->setText(user.email);
    phoneEdit->setText(user.phone);
    passwordEdit->setText(user.password);
    int idx = roleCombo->findText(user.role);
    if (idx >= 0) roleCombo->setCurrentIndex(idx);
}

User UserEditDialog::getUser() const {
    User user;
    user.name = nameEdit->text();
    user.surname = surnameEdit->text();
    user.username = usernameEdit->text();
    user.email = emailEdit->text();
    user.phone = phoneEdit->text();
    user.password = passwordEdit->text();
    user.role = roleCombo->currentText();
    return user;
}

void UserEditDialog::setEditMode(bool edit) {
    editMode = edit;
    usernameEdit->setReadOnly(edit);
}

void UserEditDialog::setRoleSelectionVisible(bool visible) {
    roleCombo->setVisible(visible);
    if (!visible) {
        if (!userRoleLabel) {
            userRoleLabel = new QLabel("Rol: user", this);
            userRoleLabel->setStyleSheet("color: #85929E; font-style: italic; margin-bottom: 8px; margin-left: 4px;");
            layout()->addWidget(userRoleLabel);
        }
        userRoleLabel->setVisible(true);
    } else if (userRoleLabel) {
        userRoleLabel->setVisible(false);
    }
}

void UserEditDialog::accept() {
    QRegularExpression emailRegex(R"(^[\w\.-]+@[\w\.-]+\.[a-zA-Z]{2,}$)");
    QRegularExpression phoneRegex(R"(^5[0-9]{9}$)");
    if (!emailRegex.match(emailEdit->text()).hasMatch()) {
        QMessageBox::warning(this, "Hata", "Geçerli bir e-posta girin.");
        return;
    }
    if (!phoneRegex.match(phoneEdit->text()).hasMatch()) {
        QMessageBox::warning(this, "Hata", "Geçerli bir telefon numarası girin (5xx...)");
        return;
    }
    QDialog::accept();
} 