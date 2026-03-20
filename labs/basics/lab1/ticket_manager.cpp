#include "ticket_manager.h"

#include <QtGlobal>

TicketManager::TicketManager(int ticket_count) {
  Reset(ticket_count);
}

void TicketManager::Reset(int ticket_count) {
  tickets_.clear();
  tickets_.reserve(ticket_count);
  for (int index = 0; index < ticket_count; ++index) {
    tickets_.push_back(Ticket{DefaultNameFor(index), Status::kDefault});
  }
}

int TicketManager::GetTicketCount() const {
  return tickets_.size();
}

bool TicketManager::IsValidIndex(int index) const {
  return index >= 0 && index < tickets_.size();
}

const TicketManager::Ticket& TicketManager::GetTicket(int index) const {
  Q_ASSERT(IsValidIndex(index));
  return tickets_[index];
}

void TicketManager::SetName(int index, const QString& name) {
  if (!IsValidIndex(index)) {
    return;
  }
  tickets_[index].name = name;
}

void TicketManager::SetStatus(int index, Status status) {
  if (!IsValidIndex(index)) {
    return;
  }
  tickets_[index].status = status;
}

void TicketManager::ToggleStatusFromView(int index) {
  if (!IsValidIndex(index)) {
    return;
  }

  if (tickets_[index].status == Status::kGreen) {
    tickets_[index].status = Status::kYellow;
    return;
  }

  tickets_[index].status = Status::kGreen;
}

int TicketManager::CountReviewed() const {
  int reviewed_count = 0;
  for (const Ticket& ticket : tickets_) {
    if (ticket.status != Status::kDefault) {
      ++reviewed_count;
    }
  }
  return reviewed_count;
}

int TicketManager::CountGreen() const {
  int green_count = 0;
  for (const Ticket& ticket : tickets_) {
    if (ticket.status == Status::kGreen) {
      ++green_count;
    }
  }
  return green_count;
}

QVector<int> TicketManager::GetAvailableForRandom(int excluded_index) const {
  QVector<int> indices;
  for (int index = 0; index < tickets_.size(); ++index) {
    if (tickets_[index].status == Status::kGreen) {
      continue;
    }
    if (index == excluded_index) {
      continue;
    }
    indices.push_back(index);
  }

  if (!indices.isEmpty()) {
    return indices;
  }

  if (IsValidIndex(excluded_index) &&
      tickets_[excluded_index].status != Status::kGreen) {
    indices.push_back(excluded_index);
  }

  return indices;
}

QString TicketManager::DefaultNameFor(int index) {
  return QString("Билет %1").arg(index + 1);
}
