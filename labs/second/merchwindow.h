#ifndef MERCHWINDOW_H
#define MERCHWINDOW_H

#include <QDialog>
#include <QDateEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

class MerchWindow : public QDialog
{
    Q_OBJECT

   public:
    MerchWindow(const QString &driverName, QWidget *parent = nullptr);

   private slots:
    void orderMerch(); // Заказ мерча

   private:
    QDateEdit *dateEdit; // Выбор даты
    QComboBox *merchComboBox; // Выбор мерча
    QLineEdit *nameLineEdit; // Имя и фамилия
    QPushButton *orderButton; // Кнопка заказа
    QLineEdit *addressLineEdit; // Адрес
};

#endif // MERCHWINDOW_H
