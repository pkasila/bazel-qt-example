#ifndef TICKETTABLEWIDGET_H
#define TICKETTABLEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFrame>
#include <QLabel>
#include <QVector>
//
#include "TicketCell.h"

class TicketTableWidget : public QTableWidget {
    Q_OBJECT

public:
    explicit TicketTableWidget(int ticketCount, QWidget *parent);

    void addTickets(int additionalTickets);

    TicketCell* getTicket(int row) const;

    int getNumberOfTickets() const;

    QVector<int> getUnrevisedTicketsRows() const;

public slots:
    void additionalPointsScored (int additionalPoints);
    void additionalGreenTicketAppeared(int ticket);

signals:
    void possibleScoreIsIncreased(int additionalPossiblePoints);
    void possibleGreenTicketsNumberIsIncreased(int additionalPossibleGreenTickets);
    void signalAdditionalPointsScored (int additionalPoints);
    void signalAdditionalGreenTicketAppeared(int ticket);

private:
    QVector<TicketCell*> tickets;
};

#endif // TICKETTABLEWIDGET_H
