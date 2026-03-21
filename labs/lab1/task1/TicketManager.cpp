#include "TicketManager.h"

#include "Ticket.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QString>
#include <algorithm>
#include <random>

void TicketManager::GenerateTickets(  // NOLINT(readability-convert-member-functions-to-static)
    int count) {
    if (count > m_tickets_.size()) {
        int current_max_id = 0;
        for (const auto& t : m_tickets_) {
            if (t.id > current_max_id) {
                current_max_id = t.id;
            }
        }

        while (m_tickets_.size() < count) {
            current_max_id++;
            m_tickets_.append(
                {current_max_id, QString("Билет %1").arg(current_max_id), TicketStatus::Default});
        }
    } else if (count < m_tickets_.size()) {
        m_tickets_.erase(
            std::remove_if(
                m_tickets_.begin(), m_tickets_.end(),
                [count](const Ticket& t) { return t.id > count; }),
            m_tickets_.end());
    }
    m_history_.clear();
}

void TicketManager::ResetNames() {  // NOLINT(readability-convert-member-functions-to-static)
    for (int i = 0; i < m_tickets_.size(); ++i) {
        m_tickets_[i].name = QString("Билет %1").arg(m_tickets_[i].id);
    }
}

void TicketManager::ResetStatuses() {  // NOLINT(readability-convert-member-functions-to-static)
    for (auto& ticket : m_tickets_) {
        ticket.status = TicketStatus::Default;
    }
    m_history_.clear();
}

void TicketManager::ShuffleTickets() {  // NOLINT(readability-convert-member-functions-to-static)
    std::random_device rd;
    auto seed = static_cast<std::mt19937::result_type>(rd());
    std::mt19937 g(
        seed);  // NOLINT(misc-const-correctness,bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
    std::shuffle(m_tickets_.begin(), m_tickets_.end(), g);
}

void TicketManager::SortTicketsById() {  // NOLINT(readability-convert-member-functions-to-static)
    std::sort(m_tickets_.begin(), m_tickets_.end(), [](const Ticket& a, const Ticket& b) {
        return a.id < b.id;
    });
}

bool TicketManager::SaveToJson(  // NOLINT(readability-convert-member-functions-to-static)
    const QString& file_path) const {
    QJsonArray array;
    for (const auto& ticket : m_tickets_) {
        QJsonObject obj;
        obj["id"] = ticket.id;
        obj["name"] = ticket.name;
        obj["status"] = static_cast<int>(ticket.status);
        array.append(obj);
    }

    QJsonObject doc_obj;
    doc_obj["tickets"] = array;

    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(QJsonDocument(doc_obj).toJson());
    return true;
}

bool TicketManager::LoadFromJson(  // NOLINT(readability-convert-member-functions-to-static)
    const QString& file_path) {
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }

    QJsonObject doc_obj = doc.object();
    QJsonArray array = doc_obj["tickets"].toArray();

    m_tickets_.clear();
    m_tickets_.reserve(array.size());

    for (int i = 0; i < array.size(); ++i) {
        QJsonObject obj = array[i].toObject();
        Ticket ticket;
        ticket.id = obj["id"].toInt();
        ticket.name = obj["name"].toString();
        ticket.status = static_cast<TicketStatus>(obj["status"].toInt());
        m_tickets_.append(ticket);
    }

    m_history_.clear();
    return true;
}

int TicketManager::GetTicketCount() const {
    return m_tickets_.size();
}

const Ticket& TicketManager::GetTicket(int index) const {
    return m_tickets_[index];
}

Ticket& TicketManager::GetTicket(int index) {
    return m_tickets_[index];
}

void TicketManager::SetTicketStatus(  // NOLINT(readability-convert-member-functions-to-static)
    int index, TicketStatus status) {
    if (index >= 0 && index < m_tickets_.size()) {
        m_tickets_[index].status = status;
    }
}

void TicketManager::SetTicketName(  // NOLINT(readability-convert-member-functions-to-static)
    int index, const QString& name) {
    if (index >= 0 && index < m_tickets_.size()) {
        m_tickets_[index].name = name;
    }
}

int TicketManager::
    GetNextRandomTicketIndex() {  // NOLINT(readability-convert-member-functions-to-static)
    QVector<int> available_indices;
    for (int i = 0; i < m_tickets_.size(); ++i) {
        if (m_tickets_[i].status == TicketStatus::Default ||
            m_tickets_[i].status == TicketStatus::Yellow) {
            available_indices.append(i);
        }
    }

    if (available_indices.isEmpty()) {
        return -1;
    }

    const int random_idx = QRandomGenerator::global()->bounded(
        available_indices.size());  // NOLINT(cppcoreguidelines-init-variables)
    return available_indices[random_idx];
}

void TicketManager::PushToHistory(int index) {
    m_history_.append(index);
}

int TicketManager::PopFromHistory() {  // NOLINT(readability-convert-member-functions-to-static)
    if (m_history_.isEmpty()) {
        return -1;
    }
    return m_history_.takeLast();
}

bool TicketManager::HasHistory() const {
    return !m_history_.isEmpty();
}

int TicketManager::GetTotalProgress()
    const {  // NOLINT(readability-convert-member-functions-to-static)
    return static_cast<int>(
        std::count_if(m_tickets_.begin(), m_tickets_.end(), [](const Ticket& t) {
            return t.status != TicketStatus::Default;
        }));
}

int TicketManager::GetGreenProgress()
    const {  // NOLINT(readability-convert-member-functions-to-static)
    return static_cast<int>(
        std::count_if(m_tickets_.begin(), m_tickets_.end(), [](const Ticket& t) {
            return t.status == TicketStatus::Green;
        }));
}
