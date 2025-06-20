#include "roomeditdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

RoomEditDialog::RoomEditDialog(QWidget* parent) : QDialog(parent), editMode(false) {
    setWindowTitle("Oda Düzenle/Ekle");
    QFormLayout* form = new QFormLayout;
    roomNoEdit = new QLineEdit;
    floorEdit = new QLineEdit;
    typeCombo = new QComboBox;
    typeCombo->addItems({"standart", "suit", "deluxe"});
    statusCombo = new QComboBox;
    statusCombo->addItems({"available", "reserved", "occupied", "cleaning"});
    form->addRow("Oda No:", roomNoEdit);
    form->addRow("Kat:", floorEdit);
    form->addRow("Tip:", typeCombo);
    form->addRow("Durum:", statusCombo);
    okButton = new QPushButton("Kaydet");
    cancelButton = new QPushButton("İptal");
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(okButton);
    btnLayout->addWidget(cancelButton);
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(form);
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void RoomEditDialog::setRoom(const Room& room) {
    roomNoEdit->setText(QString::number(room.room_no));
    floorEdit->setText(QString::number(room.floor));
    int idxType = typeCombo->findText(room.type);
    if (idxType >= 0) typeCombo->setCurrentIndex(idxType);
    int idxStatus = statusCombo->findText(room.status);
    if (idxStatus >= 0) statusCombo->setCurrentIndex(idxStatus);
}

Room RoomEditDialog::getRoom() const {
    Room room;
    room.room_no = roomNoEdit->text().toInt();
    room.floor = floorEdit->text().toInt();
    room.type = typeCombo->currentText();
    room.status = statusCombo->currentText();
    return room;
}

void RoomEditDialog::setEditMode(bool edit) {
    editMode = edit;
    roomNoEdit->setReadOnly(edit);
} 