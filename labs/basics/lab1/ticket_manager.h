#ifndef TICKET_MANAGER_H_
#define TICKET_MANAGER_H_

#include <QString>
#include <QVector>

class TicketManager {
 public:
  enum class Status {
    kDefault = 0,
    kYellow = 1,
    kGreen = 2,
  };

  struct Ticket {
    QString name;
    Status status = Status::kDefault;
  };

  explicit TicketManager(int ticket_count = 10);

  void Reset(int ticket_count);
  int GetTicketCount() const;
  bool IsValidIndex(int index) const;

  const Ticket& GetTicket(int index) const;

  void SetName(int index, const QString& name);
  void SetStatus(int index, Status status);
  void ToggleStatusFromView(int index);

  int CountReviewed() const;
  int CountGreen() const;
  QVector<int> GetAvailableForRandom(int excluded_index = -1) const;

  static QString DefaultNameFor(int index);

 private:
  QVector<Ticket> tickets_;
};

#endif  // TICKET_MANAGER_H_
