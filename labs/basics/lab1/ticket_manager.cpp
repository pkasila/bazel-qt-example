#include "ticket_manager.h"
#include <algorithm>
#include <random>

TicketManager::TicketManager(int capacity, QObject* parent)
    : QObject(parent), tickets_(capacity) {
  for (auto& ticket : tickets_) {
    ticket.status = Status::kDefault;
  }
}

void TicketManager::SetCapacity(int capacity) {
  tickets_.resize(capacity);
  for (auto& ticket : tickets_) {
    ticket.status = Status::kDefault;
    ticket.name.clear();
  }
}

int TicketManager::GetCapacity() const { return tickets_.size(); }

int TicketManager::GetTicketIndex(int ticket_id) const { return ticket_id; }

TicketManager::Status TicketManager::GetStatus(int ticket_id) const {
  if (ticket_id < 0 || ticket_id >= static_cast<int>(tickets_.size())) {
    return Status::kDefault;
  }
  return tickets_[ticket_id].status;
}

void TicketManager::SetStatus(int ticket_id, Status status) {
  if (ticket_id >= 0 && ticket_id < static_cast<int>(tickets_.size())) {
    tickets_[ticket_id].status = status;
  }
}

QString TicketManager::GetName(int ticket_id) const {
  if (ticket_id < 0 || ticket_id >= static_cast<int>(tickets_.size())) {
    return QString();
  }
  return tickets_[ticket_id].name;
}

void TicketManager::SetName(int ticket_id, const QString& name) {
  if (ticket_id >= 0 && ticket_id < static_cast<int>(tickets_.size())) {
    tickets_[ticket_id].name = name;
  }
}

int TicketManager::GetTotalProgress() const {
  int count = 0;
  for (const auto& ticket : tickets_) {
    if (ticket.status != Status::kDefault) {
      count++;
    }
  }
  return count;
}

int TicketManager::GetMasteredProgress() const {
  int count = 0;
  for (const auto& ticket : tickets_) {
    if (ticket.status == Status::kMastered) {
      count++;
    }
  }
  return count;
}

std::vector<int> TicketManager::GetNonMasteredIndices() const {
  std::vector<int> indices;
  for (size_t i = 0; i < tickets_.size(); ++i) {
    if (tickets_[i].status != Status::kMastered) {
      indices.push_back(i);
    }
  }
  return indices;
}

int TicketManager::GetRandomNonMasteredIndex() const {
  auto indices = GetNonMasteredIndices();
  if (indices.empty()) {
    return -1;
  }
  std::mt19937 generator(std::random_device{}());
  std::uniform_int_distribution<> distribution(0, indices.size() - 1);
  return indices[distribution(generator)];
}
