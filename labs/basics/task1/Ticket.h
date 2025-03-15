#ifndef TICKET_H
#define TICKET_H

#include <QString>
#include <QColor>


class Ticket {
public:
    enum Status { Default, InProgress, Done };

    Ticket(const QString &text, Status status = Default, int number = 0)
        : text(text), status(status), number(number) {}

    Ticket() : text("not yet"), status(Status::Default), number(0) {}

    QString getText() const { return text; }
    Status getStatus() const { return status; }

    void setStatus(Status newStatus) { status = newStatus; }
    void setText(QString newText) { text = newText; }

    QColor getStatusColor() const {
        switch (status) {
            case Default:
                return Qt::gray;
            case InProgress:
                return Qt::yellow;
            case Done:
                return Qt::green;
            default:
                return Qt::white;
        }
    }

    QString getStatusText() const {
        switch (status) {
            case Default:
                return "Default";
            case InProgress:
                return "In Progress";
            case Done:
                return "Done";
            default: return "Unknown";
        }
    }

    void cycleStatus() {
        switch (status) {
            case Default:
                status = InProgress;
            break;
            case InProgress:
                status = Done;
            break;
            case Done:
                status = InProgress;
            break;
        }
    }

private:
    int number;
    QString text;
    Status status;
};

#endif //TICKET_H
