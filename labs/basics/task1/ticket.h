#pragma once
#include <QString>
#include <cstdint>

enum TicketStatus : uint8_t {
    kNone = 0,
    kInProcess = 1,
    kDone = 2
};

class Ticket {
public:
    Ticket() = delete;
    explicit Ticket(int num) : num_{num}, stat_{TicketStatus::kNone}, name_{QString("Билет %1").arg(num)} {
    }
    void ChangeStat();
    void ChangeName(const QString& new_name);
    [[nodiscard]] TicketStatus GetStat() const {
        return stat_;
    }
    [[nodiscard]] int GetNum() const {
        return num_;
    }
    [[nodiscard]] const QString GetName() const {
        return name_;
    }
    void SetStatus(int stat) {
        stat_ = static_cast<TicketStatus>(stat);
    }
    QString GetHint() const;
    void SetHint(const QString& hint);
private:
    int num_;
    TicketStatus stat_;
    QString name_;
    QString hint_;
};
