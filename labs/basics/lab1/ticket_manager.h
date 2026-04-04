#ifndef TICKET_MANAGER_H
#define TICKET_MANAGER_H

#include <QObject>
#include <QString>
#include <vector>

class TicketManager : public QObject {
  Q_OBJECT

 public:
  enum class Status {
    kDefault = 0,
    kRepeated = 1,
    kMastered = 2
  };

  explicit TicketManager(int capacity = 0, QObject* parent = nullptr);

  void SetCapacity(int capacity);
  int GetCapacity() const;
  int GetTicketIndex(int ticket_id) const;

  Status GetStatus(int ticket_id) const;
  void SetStatus(int ticket_id, Status status);

  QString GetName(int ticket_id) const;
  void SetName(int ticket_id, const QString& name);

  int GetTotalProgress() const;
  int GetMasteredProgress() const;

  std::vector<int> GetNonMasteredIndices() const;
  int GetRandomNonMasteredIndex() const;

 private:
  struct Ticket {
    Status status = Status::kDefault;
    QString name;
  };

  std::vector<Ticket> tickets_;
};

#endif  // TICKET_MANAGER_H
