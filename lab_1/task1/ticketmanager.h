#ifndef TICKETMANAGER_H
#define TICKETMANAGER_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QRandomGenerator>

class TicketManager : public QObject
{
    Q_OBJECT

public:
    enum Status { Default = 0, Yellow, Green };

    struct Ticket {
        int number;
        QString name;
        Status status;

        Ticket() : number(0), name(""), status(Default) {}
        Ticket(int num) : number(num), name(tr("Билет %1").arg(num)), status(Default) {}
    };

    explicit TicketManager(QObject *parent = nullptr);

    void setTicketCount(int count);
    int getTicketCount() const;

    QVector<Ticket>& getTickets();
    const QVector<Ticket>& getTickets() const;

    void updateTicketName(int index, const QString &newName);
    void updateTicketStatus(int index, Status newStatus);

    QVector<int> getNonGreenTicketIndices() const;
    int getRandomNonGreenIndex() const;

    int getTotalProgress() const;
    int getGreenProgress() const;

signals:
    void ticketsChanged();

private:
    QVector<Ticket> m_tickets;
};

#endif // TICKETMANAGER_H