#ifndef TICKET_H
#define TICKET_H

#include <QString>                                   // NOLINT

enum class TicketStatus { Default, Yellow, Green };  // NOLINT(performance-enum-size)

struct Ticket {
    int id{0};
    QString name{};
    TicketStatus status{TicketStatus::Default};
};

#endif  // TICKET_H
