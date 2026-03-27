#include "ticketwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDate>

TicketWindow::TicketWindow(const QString &trackName, const QDate &eventDate, QWidget *parent)
    : QDialog(parent), eventDate(eventDate)
{
    resize(470, this->height());

    setWindowTitle(trackName);

            // Основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

            // Поле для выбора типа билета
    ticketTypeComboBox = new QComboBox(this);
    ticketTypeComboBox->addItem("Детский");
    ticketTypeComboBox->addItem("Обычный");
    ticketTypeComboBox->addItem("VIP");
    mainLayout->addWidget(new QLabel("Тип билета:"));
    mainLayout->addWidget(ticketTypeComboBox);

            // Поле для количества билетов
    ticketCountSpinBox = new QSpinBox(this);
    ticketCountSpinBox->setRange(1, 10);
    mainLayout->addWidget(new QLabel("Количество билетов:"));
    mainLayout->addWidget(ticketCountSpinBox);

            // Поле для имени и фамилии
    nameLineEdit = new QLineEdit(this);
    nameLineEdit->setPlaceholderText("Введите ваше имя и фамилию");
    mainLayout->addWidget(new QLabel("Имя и фамилия:"));
    mainLayout->addWidget(nameLineEdit);

            // Кнопка покупки
    buyButton = new QPushButton("Купить билет", this);
    connect(buyButton, &QPushButton::clicked, this, &TicketWindow::buyTicket);
    mainLayout->addWidget(buyButton);
    mainLayout->addStretch(1);
}

void TicketWindow::buyTicket()
{
    // Проверяем, прошла ли дата гран-при
    if (eventDate < QDate::currentDate()) {
        QMessageBox::warning(this, "Ошибка", "Покупка невозможна: событие уже состоялось.");
        return;
    }

            // Формируем сообщение о покупке
    QString message = QString("Вы купили %1 билет(ов) типа %2 на %3.\nИмя: %4")
                          .arg(ticketCountSpinBox->value())
                          .arg(ticketTypeComboBox->currentText())
                          .arg(windowTitle())
                          .arg(nameLineEdit->text());
    QMessageBox::information(this, "Покупка билета", message);
    close(); // Закрываем окно после покупки
}
