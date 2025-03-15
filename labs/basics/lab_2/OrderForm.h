#ifndef ORDERFORM_H
#define ORDERFORM_H

#include <QDialog>
#include <QCalendarWidget>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>

class OrderForm : public QDialog {
    Q_OBJECT

public:
    explicit OrderForm(QWidget *parent = nullptr);

    QString getDeliveryDate() const;
    QString getCity() const;
    QString getPaymentMethod() const;
    QString getCustomerName() const;
    bool isConfirmed() const;

private:
    QCalendarWidget *calendar;
    QComboBox *cityCombo;
    QComboBox *paymentCombo;
    QPushButton *submitButton;
    QPushButton *cancelButton;
    QCheckBox *confirmCheckBox;
    QLineEdit *nameInput;
};

#endif // ORDERFORM_H
