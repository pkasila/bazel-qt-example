#include "ticketlist.h"
#include <QSpinBox>
#include <QRandomGenerator>

TicketList::TicketList(QWidget *parent) : QListWidget(parent)
{
    connect(this, &QListWidget::itemClicked, this, &TicketList::handleItemClicked);
    connect(this, &QListWidget::itemDoubleClicked, this, &TicketList::handleItemDoubleClicked);
}

void TicketList::updateTickets(int count) {
    this->clear();

    for (int i = 1; i <= count; ++i) {
        addItem(new TicketItem(i));
    }
    emit progressUpdated();

}

void TicketList::handleItemClicked(QListWidgetItem *item) {
    TicketItem *ticket = dynamic_cast<TicketItem *>(item);
    if (ticket) {
        emit ticketSelected(ticket);

        if (history.isEmpty() || history.last() != ticket) {
            history.append(ticket);
            iterOfHist = history.size();
        }
    }
    emit progressUpdated();
}


void TicketList::handleItemDoubleClicked(QListWidgetItem *item) {
    TicketItem *ticket = dynamic_cast<TicketItem *>(item);
    if (ticket) {
        ticket->updateItemStatus();
    }
    emit progressUpdated();
}

void TicketList::randomTick(){
    QList<TicketItem*> neededTickets;

    for(int i = 0; i < count(); ++i){
        TicketItem *ticket = dynamic_cast<TicketItem*>(item(i));
        if(ticket && (ticket->getStatus() == TicketItem::Default
            ||ticket->getStatus() == TicketItem::NeedReview)){
            neededTickets.append(ticket);
        }
    }
    if (neededTickets.isEmpty()) {
        return;
    }
    int randomIndex = QRandomGenerator::global()->bounded(neededTickets.size());

    TicketItem *randomTicket = neededTickets[randomIndex];
    handleItemClicked(randomTicket);
}

void TicketList::previousQues(){
    if (iterOfHist > 0) {
        iterOfHist--;
        TicketItem *ticket = history[iterOfHist];

        if (ticket) {
            emit ticketSelected(ticket);
        }
    }
}

