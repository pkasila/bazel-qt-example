#include "OrderForm.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
OrderForm::OrderForm(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Оформление заказа");
    setFixedSize(400, 350);

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Ваше имя:"));
    nameInput = new QLineEdit(this);
    layout->addWidget(nameInput);

    layout->addWidget(new QLabel("Дата доставки:"));
    calendar = new QCalendarWidget();
    calendar->setMinimumDate(QDate::currentDate());
    layout->addWidget(calendar);

    layout->addWidget(new QLabel("Город:"));
    cityCombo = new QComboBox();
    cityCombo->addItems({"Москва", "Санкт-Петербург", "Минск", "Витебск"});
    layout->addWidget(cityCombo);

    layout->addWidget(new QLabel("Способ оплаты:"));
    paymentCombo = new QComboBox();
    paymentCombo->addItems({"Наличные курьеру", "Картой онлайн", "Перевод на счет"});
    layout->addWidget(paymentCombo);

    confirmCheckBox = new QCheckBox("Подтверждаю условия покупки", this);
    layout->addWidget(confirmCheckBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    submitButton = new QPushButton("Оформить");
    cancelButton = new QPushButton("Отмена");

    submitButton->setEnabled(false);

    connect(nameInput, &QLineEdit::textChanged, this, [=](const QString &text) {
        submitButton->setEnabled(
            !text.trimmed().isEmpty() &&
            confirmCheckBox->isChecked()
        );
    });

    connect(confirmCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
        submitButton->setEnabled(
            state == Qt::Checked &&
            !nameInput->text().trimmed().isEmpty()
        );
    });

    buttonLayout->addWidget(submitButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    connect(submitButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}
bool OrderForm::isConfirmed() const {
    return confirmCheckBox->isChecked();
}

QString OrderForm::getDeliveryDate() const {
    return calendar->selectedDate().toString("dd.MM.yyyy");
}

QString OrderForm::getCity() const {
    return cityCombo->currentText();
}

QString OrderForm::getPaymentMethod() const {
    return paymentCombo->currentText();
}

QString OrderForm::getCustomerName() const {
    return nameInput->text().trimmed();
}
