#include "TicketTableWidget.h"

TicketTableWidget::TicketTableWidget(int ticketCount,
    QWidget *parent = nullptr) : QTableWidget(ticketCount, 1, parent) {
    verticalHeader()->setVisible(true);
    horizontalHeader()->setVisible(false);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setStyleSheet(Styles::ticketTableStyle);
}

void TicketTableWidget::addTickets(int additionalTickets) {
    emit possibleGreenTicketsNumberIsIncreased(additionalTickets);
    emit possibleScoreIsIncreased(2 * additionalTickets); //each ticket can give 2 points
    int rows = rowCount();
    for (int i = 0; i < additionalTickets; ++i) {
        TicketCell *newTicket = new TicketCell(rows + 1,
            Formats::defaultTicketNameFormat.arg(QString::number(rows + 1)), Status::Red, this);
        insertRow(rows);
        setRowHeight(rows, newTicket->sizeHint().height());
        setCellWidget(rows, 0, newTicket);
        connect(newTicket, &TicketCell::additionalPointsScored,
                this, &TicketTableWidget::additionalPointsScored);
        connect(newTicket, &TicketCell::additionalGreenTicketAppeared,
                this, &TicketTableWidget::additionalGreenTicketAppeared);
        tickets.push_back(newTicket);
        newTicket->show();
        rows++;
    }
}

int TicketTableWidget::getNumberOfTickets() const {
    return tickets.size();
}

TicketCell* TicketTableWidget::getTicket(int row) const { //ticketNumber == row + 1
    return tickets[row];
}

QVector<int> TicketTableWidget::getUnrevisedTicketsRows() const {
    QVector<int> unrevisedTicketsRows;
    for (const auto& ticket: tickets) {
        if (ticket->getTicketStatus() != Status::Green) {
            unrevisedTicketsRows.append(ticket->getTicketNumber() - 1);
        }
    }
    return unrevisedTicketsRows;
}

void TicketTableWidget::additionalPointsScored (int additionalPoints) {
    emit signalAdditionalPointsScored(additionalPoints);
}

void TicketTableWidget::additionalGreenTicketAppeared(int ticket) {
    emit signalAdditionalGreenTicketAppeared(ticket);
}
