#ifndef TICKETWINDOW_H
#define TICKETWINDOW_H

#include <QDialog>
#include <QDate>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>

class TicketWindow : public QDialog
{
    Q_OBJECT

   public:
    TicketWindow(const QString &trackName, const QDate &eventDate, QWidget *parent = nullptr);

   private slots:
    void buyTicket(); // Покупка билета

   private:
    QComboBox *ticketTypeComboBox; // Тип билета
    QSpinBox *ticketCountSpinBox;  // Количество билетов
    QLineEdit *nameLineEdit;       // Имя и фамилия
    QPushButton *buyButton;        // Кнопка покупки
    QDate eventDate;               // Дата гран-при
};

#endif // TICKETWINDOW_H
