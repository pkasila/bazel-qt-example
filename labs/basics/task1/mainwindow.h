#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QSpinBox>
#include <QMap>
#include <QTableWidget>

enum class TicketStatus {
    Default,
    Yellow,
    Green
};

class Ticket {
private:
    int number_;
    TicketStatus status_;
    QString name_;
    QString txt_;
public:
    Ticket(int number)
        : number_(number), status_(TicketStatus::Default), name_("Билет " + QString::number(number)) {}

    Ticket() : number_(0), status_(TicketStatus::Default), name_("Билет 0") {}

    int getNumber() const {
        return number_;
    }

    void setNumber(int newNumber) {
        number_ = newNumber;
        name_ = "Билет " + QString::number(number_);
    }

    QString getName() const {
        return name_;
    }

    void setName(const QString& newName) {
        name_ = newName;
    }

    TicketStatus getStatus() const {
        return status_;
    }

    void setStatus(TicketStatus newStatus) {
        status_ = newStatus;
    }

    QString getText() const {
        return txt_;
    }

    void setText(const QString& text) {
        txt_ = text;
    }
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_spinBoxTickets_valueChanged(int arg1);
    void on_tableWidget_itemClicked(QTableWidgetItem *item);
    void on_btnNextTicket_clicked();
    void on_btnPrevTicket_clicked();
    void on_comboTicketStatus_currentIndexChanged(int index);
    void on_btnRandomTicket_clicked();
    void on_ticketName_textChanged();
    void on_TicketTXT_textChanged();
    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

    void setDarkTheme();
    void setLightTheme();

private:
    Ui::MainWindow *ui;
    QMap<int, Ticket*> tickets_;
    QVector<Ticket*> unexecutedTickets_;
    QVector<Ticket*> repeatedTickets_;
    int completedTickets_ = 0;

    void updateTicketInTable(int ticketNumber, const Ticket &ticket);
    void updateProgressBar();
    void selectTicketInTableByName(Ticket* ticket);
    void updateTicketStatus(Ticket* ticket, TicketStatus newStatus);
    void refreshTicketStatusColors();
};

#endif // MAINWINDOW_H
