#ifndef TICKETLIST_H
#define TICKETLIST_H

#include <QListWidget>
#include "ticketitem.h"
#include <vector>

class TicketList : public QListWidget
{
    Q_OBJECT

public:
    explicit TicketList(QWidget *parent = nullptr);
    void updateTickets(int count);
    void randomTick();
    void previousQues();
signals:
    void ticketSelected(TicketItem *ticket);
    void progressUpdated();


private slots:
    void handleItemClicked(QListWidgetItem *item);
    void handleItemDoubleClicked(QListWidgetItem *item);

private:
    QVector<TicketItem *> history;
    int iterOfHist = 0;

};

#endif // TICKETLIST_H
