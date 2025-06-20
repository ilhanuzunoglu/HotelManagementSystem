#include "invoicedialog.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QDate>

InvoiceDialog::InvoiceDialog(const Reservation& r, QWidget* parent) : QDialog(parent) {
    setWindowTitle("Fatura Detayı");
    detailsEdit = new QTextEdit;
    detailsEdit->setReadOnly(true);
    closeButton = new QPushButton("Kapat");
    QString cardMasked = r.card_number;
    if (cardMasked.length() > 4) cardMasked = QString("**** **** **** %1").arg(r.card_number.right(4));
    QString text = QString(
        "<b>Otel:</b> Grand Hotel<br>"
        "<b>Kullanıcı:</b> %1<br>"
        "<b>Oda No:</b> %2<br>"
        "<b>Oda Tipi:</b> %3<br>"
        "<b>Giriş Tarihi:</b> %4<br>"
        "<b>Çıkış Tarihi:</b> %5<br>"
        "<b>Fiyat:</b> %6 TL<br>"
        "<b>Kredi Kartı:</b> %7<br>"
        "<b>Ödeme Durumu:</b> %8<br>"
        "<b>Rezervasyon Tarihi:</b> %9<br>"
    )
    .arg(r.username)
    .arg(r.room_no)
    .arg(r.status)
    .arg(r.checkin)
    .arg(r.checkout)
    .arg(r.price, 0, 'f', 2)
    .arg(cardMasked)
    .arg(r.payment_status)
    .arg(r.created_at);
    detailsEdit->setHtml(text);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(detailsEdit);
    layout->addWidget(closeButton);
    setLayout(layout);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
} 