#include "ticket_manager.h"

#include <algorithm>

TicketManager::TicketManager(int ticket_count)
    : tickets_(), generator_(std::random_device{}()) {
  Reset(ticket_count);
}

void TicketManager::Reset(int ticket_count) {
  tickets_.clear();
  tickets_.reserve(ticket_count);
  for (int index = 0; index < ticket_count; ++index) {
    tickets_.push_back({GetDefaultName(index), TicketStatus::kDefault});
  }
}

int TicketManager::GetCount() const { return static_cast<int>(tickets_.size()); }

QString TicketManager::GetDefaultName(int index) const {
  return QString::fromUtf8("Билет %1").arg(index + 1);
}

QString TicketManager::GetName(int index) const {
  if (!IsValidIndex(index)) {
    return QString();
  }
  return tickets_[index].name;
}

TicketStatus TicketManager::GetStatus(int index) const {
  if (!IsValidIndex(index)) {
    return TicketStatus::kDefault;
  }
  return tickets_[index].status;
}

void TicketManager::SetName(int index, const QString& name) {
  if (!IsValidIndex(index)) {
    return;
  }
  tickets_[index].name = name;
}

void TicketManager::SetStatus(int index, TicketStatus status) {
  if (!IsValidIndex(index)) {
    return;
  }
  tickets_[index].status = status;
}

void TicketManager::ToggleStatusByDoubleClick(int index) {
  if (!IsValidIndex(index)) {
    return;
  }

  if (tickets_[index].status == TicketStatus::kGreen) {
    tickets_[index].status = TicketStatus::kYellow;
    return;
  }

  tickets_[index].status = TicketStatus::kGreen;
}

int TicketManager::GetTotalProgress() const {
  return static_cast<int>(std::count_if(
      tickets_.begin(), tickets_.end(), [](const TicketData& ticket) {
        return ticket.status != TicketStatus::kDefault;
      }));
}

int TicketManager::GetGreenProgress() const {
  return static_cast<int>(std::count_if(
      tickets_.begin(), tickets_.end(), [](const TicketData& ticket) {
        return ticket.status == TicketStatus::kGreen;
      }));
}

bool TicketManager::HasEligibleTickets() const {
  return std::any_of(tickets_.begin(), tickets_.end(),
                     [](const TicketData& ticket) {
                       return ticket.status != TicketStatus::kGreen;
                     });
}

int TicketManager::PickRandomEligible(int current_index) {
  std::vector<int> eligible_indices;
  for (int index = 0; index < GetCount(); ++index) {
    if (tickets_[index].status != TicketStatus::kGreen) {
      eligible_indices.push_back(index);
    }
  }

  if (eligible_indices.empty()) {
    return -1;
  }

  if (eligible_indices.size() > 1) {
    eligible_indices.erase(
        std::remove(eligible_indices.begin(), eligible_indices.end(),
                    current_index),
        eligible_indices.end());
  }

  if (eligible_indices.empty()) {
    return -1;
  }

  std::uniform_int_distribution<int> distribution(
      0, static_cast<int>(eligible_indices.size()) - 1);
  return eligible_indices[distribution(generator_)];
}

bool TicketManager::IsValidIndex(int index) const {
  return index >= 0 && index < GetCount();
}
