#ifndef TICKET_H
#define TICKET_H

#include <QColor>
#include <QString>

enum class TicketStatus : int8_t { Default, Yellow, Green };

class Ticket {
   public:
    Ticket(size_t index, QString name, TicketStatus status, QString hint);
    [[nodiscard]] QColor GetStatusColor() const;
    [[nodiscard]] std::tuple<size_t, QString, TicketStatus, QString> GetPrivate() const;
    void Rename(const QString& new_name);
    void ChangeStatus(TicketStatus new_status);
    void SetHint(const QString& hint);

   private:
    std::size_t index_;
    QString name_;
    TicketStatus status_;
    QString hint_;
};
#endif