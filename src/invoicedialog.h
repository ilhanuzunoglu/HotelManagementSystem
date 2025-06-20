#ifndef INVOICEDIALOG_H
#define INVOICEDIALOG_H
#include <QDialog>
#include "database.h"
class QLabel;
class QTextEdit;
class QPushButton;
class InvoiceDialog : public QDialog {
    Q_OBJECT
public:
    InvoiceDialog(const Reservation& reservation, QWidget* parent = nullptr);
private:
    QTextEdit* detailsEdit;
    QPushButton* closeButton;
};
#endif // INVOICEDIALOG_H 