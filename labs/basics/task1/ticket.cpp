#include "ticket.h"

#include <tuple>

Ticket::Ticket(size_t index, QString name, TicketStatus status, QString hint)
    : index_(index), name_(std::move(name)), status_(status), hint_(std::move(hint)) {
}

QColor Ticket::GetStatusColor() const {
    switch (status_) {
        case TicketStatus::Default:
            return Qt::gray;
        case TicketStatus::Yellow:
            return Qt::yellow;
        case TicketStatus::Green:
            return Qt::green;
    }
    return Qt::white;
}

std::tuple<size_t, QString, TicketStatus, QString> Ticket::GetPrivate() const {
    return {index_, name_, status_, hint_};
}

void Ticket::Rename(const QString& new_name) {
    name_ = new_name;
}

void Ticket::ChangeStatus(TicketStatus new_status) {
    status_ = new_status;
}

void Ticket::SetHint(const QString& hint) {
    hint_ = hint;
}