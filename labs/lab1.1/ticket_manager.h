#ifndef TICKET_MANAGER_H_
#define TICKET_MANAGER_H_

#include <QString>

#include <random>
#include <vector>

enum class TicketStatus {
  kDefault = 0,
  kYellow = 1,
  kGreen = 2,
};

class TicketManager {
 public:
  explicit TicketManager(int ticket_count = 20);

  void Reset(int ticket_count);
  int GetCount() const;
  QString GetDefaultName(int index) const;
  QString GetName(int index) const;
  TicketStatus GetStatus(int index) const;
  void SetName(int index, const QString& name);
  void SetStatus(int index, TicketStatus status);
  void ToggleStatusByDoubleClick(int index);
  int GetTotalProgress() const;
  int GetGreenProgress() const;
  bool HasEligibleTickets() const;
  int PickRandomEligible(int current_index);

 private:
  struct TicketData {
    QString name;
    TicketStatus status;
  };

  bool IsValidIndex(int index) const;

  std::vector<TicketData> tickets_;
  std::mt19937 generator_;
};

#endif
