#include "merchwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

MerchWindow::MerchWindow(const QString &driverName, QWidget *parent)
    : QDialog(parent)
{
    resize(470, this->height());

    setWindowTitle(driverName);

    // Основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Поле для выбора даты
    dateEdit = new QDateEdit(this);
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setMinimumDate(QDate::currentDate());
    mainLayout->addWidget(new QLabel("Дата доставки:"));
    mainLayout->addWidget(dateEdit);

    // Поле для выбора мерча
    merchComboBox = new QComboBox(this);
    merchComboBox->addItem("Майка");
    merchComboBox->addItem("Кепка");
    merchComboBox->addItem("Мини-болид");
    merchComboBox->addItem("Флаг команды");
    mainLayout->addWidget(new QLabel("Выберите мерч:"));
    mainLayout->addWidget(merchComboBox);

    // Поле для имени и фамилии
    nameLineEdit = new QLineEdit(this);
    nameLineEdit->setPlaceholderText("Введите ваше имя и фамилию");
    mainLayout->addWidget(new QLabel("Имя и фамилия:"));
    mainLayout->addWidget(nameLineEdit);

    // Поле для адреса
    addressLineEdit = new QLineEdit(this);
    addressLineEdit->setPlaceholderText("Введите ваш адрес");
    mainLayout->addWidget(new QLabel("Адрес доставки:"));
    mainLayout->addWidget(addressLineEdit);

    // Кнопка заказа
    orderButton = new QPushButton("Заказать мерч", this);
    connect(orderButton, &QPushButton::clicked, this, &MerchWindow::orderMerch);
    mainLayout->addWidget(orderButton);
    mainLayout->addStretch(1);
}

void MerchWindow::orderMerch()
{
    QString message = QString("Заказ мерча:\n"
                          "Мерч: %1\n"
                          "Дата доставки: %2\n"
                          "Имя и фамилия: %3\n"
                          "Адрес доставки: %4")
                          .arg(merchComboBox->currentText())
                          .arg(dateEdit->date().toString("dd.MM.yyyy"))
                          .arg(nameLineEdit->text())
                          .arg(addressLineEdit->text());
    QMessageBox::information(this, "Заказ мерча", message);
    close(); // Закрываем окно после заказа
}
