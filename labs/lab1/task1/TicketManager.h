#ifndef TICKETMANAGER_H
#define TICKETMANAGER_H

#include "Ticket.h"

#include <QList>
#include <QString>
#include <QVector>

class TicketManager {
   public:
    TicketManager() = default;

    void GenerateTickets(int count);
    void ResetNames();
    void ResetStatuses();
    void ShuffleTickets();
    void SortTicketsById();

    [[nodiscard]] bool SaveToJson(const QString& file_path) const;
    [[nodiscard]] bool LoadFromJson(const QString& file_path);

    [[nodiscard]] int GetTicketCount() const;
    [[nodiscard]] const Ticket& GetTicket(int index) const;
    [[nodiscard]] Ticket& GetTicket(int index);

    void SetTicketStatus(int index, TicketStatus status);
    void SetTicketName(int index, const QString& name);

    [[nodiscard]] int GetNextRandomTicketIndex();

    void PushToHistory(int index);
    [[nodiscard]] int PopFromHistory();
    [[nodiscard]] bool HasHistory() const;

    [[nodiscard]] int GetTotalProgress() const;
    [[nodiscard]] int GetGreenProgress() const;

   private:
    QVector<Ticket> m_tickets_{};
    QList<int> m_history_{};
};

#endif  // TICKETMANAGER_H
