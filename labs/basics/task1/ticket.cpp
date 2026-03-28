#include "ticket.h"
void Ticket::ChangeName(const QString& name) {
    name_ = name;
}
void Ticket::ChangeStat() {
    if (stat_ == TicketStatus::kNone || stat_ == TicketStatus::kInProcess) {
        stat_ = TicketStatus::kDone;
        return;
    }
    if (stat_ == TicketStatus::kDone) {
        stat_ = TicketStatus::kInProcess;
        return;
    }
}

QString Ticket::GetHint() const {
    return hint_;
}

void Ticket::SetHint(const QString& hint) {
    hint_ = hint;
}
