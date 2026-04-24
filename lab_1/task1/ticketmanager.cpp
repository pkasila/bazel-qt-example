#include "ticketmanager.h"
#include <QRandomGenerator>

TicketManager::TicketManager(QObject *parent)
    : QObject(parent)
{
}

void TicketManager::setTicketCount(int count)
{
    if (count <= 0) {
        m_tickets.clear();
    } else {
        m_tickets.resize(count);
        for (int i = 0; i < count; ++i) {
            m_tickets[i] = Ticket(i + 1);
        }
    }
    emit ticketsChanged();
}

int TicketManager::getTicketCount() const
{
    return m_tickets.size();
}

QVector<TicketManager::Ticket>& TicketManager::getTickets()
{
    return m_tickets;
}

const QVector<TicketManager::Ticket>& TicketManager::getTickets() const
{
    return m_tickets;
}

void TicketManager::updateTicketName(int index, const QString &newName)
{
    if (index >= 0 && index < m_tickets.size()) {
        m_tickets[index].name = newName.isEmpty() ? tr("Билет %1").arg(m_tickets[index].number) : newName;
        emit ticketsChanged();
    }
}

void TicketManager::updateTicketStatus(int index, Status newStatus)
{
    if (index >= 0 && index < m_tickets.size()) {
        m_tickets[index].status = newStatus;
        emit ticketsChanged();
    }
}

QVector<int> TicketManager::getNonGreenTicketIndices() const
{
    QVector<int> indices;
    for (int i = 0; i < m_tickets.size(); ++i) {
        if (m_tickets[i].status != Green) {
            indices.append(i);
        }
    }
    return indices;
}

int TicketManager::getRandomNonGreenIndex() const
{
    auto nonGreenIndices = getNonGreenTicketIndices();
    if (nonGreenIndices.isEmpty())
        return -1;
    
    int randomIndex = QRandomGenerator::global()->bounded(nonGreenIndices.size());
    return nonGreenIndices[randomIndex];
}

int TicketManager::getTotalProgress() const
{
    int count = 0;
    for (const auto& ticket : m_tickets) {
        if (ticket.status != Default)
            count++;
    }
    return count;
}

int TicketManager::getGreenProgress() const
{
    int count = 0;
    for (const auto& ticket : m_tickets) {
        if (ticket.status == Green)
            count++;
    }
    return count;
}