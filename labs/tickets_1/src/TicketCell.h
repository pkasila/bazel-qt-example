#ifndef TICKETCELL_H
#define TICKETCELL_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QFrame>
#include <QHBoxLayout>
//
#include "ProjectConstants.h"


class TicketCell : public QWidget {
    Q_OBJECT

public:
    explicit TicketCell(int ticketNumber, const QString& ticketName,
               int ticketStatus, QWidget *parent = nullptr);

    //Setters
    void setTicketStatus(int ticketStatus);

    void setTicketName(const QString& ticketName);

    void setTicketText(const QString& ticketText);

    //Getters
    int getTicketStatus() const;

    QString getTicketName() const;

    QString getTicketText() const;

    int getTicketNumber() const;

signals:
    void additionalPointsScored (int additionalPoints);
    void additionalGreenTicketAppeared(int ticket);

private:
    QLabel *ticketNameLabel;
    QFrame *statusFlag;

private:
    int ticketNumber;
    QString ticketName;
    int ticketStatus; //0 - red, 1 - yellow, 2 - green
    QString ticketText;
};

#endif // TICKETCELL_H
