#include <QColor>
#ifndef TICKET_H
#define TICKET_H

#include <string>


class Ticket {
public:
    enum Status: uint8_t {
        DEFAULT,
        YELLOW,
        GREEN
    };

    Ticket() {
    }

    Ticket(std::string name): number_(-1), name_(name), status_(DEFAULT) {
    }

    Ticket(int number, std::string name): number_(number), name_(name), status_(DEFAULT) {
    }

    Ticket(int number, std::string name, Status status_): number_(number), name_(name), status_(status_) {
    }

    ~Ticket() {
    }

    int getNumber() const { return number_; }
    std::string getName() { return name_; }
    Status getStatus() { return status_; }

    void setNumber(int number) { this->number_ = number; }
    void setName(std::string name) { this->name_ = name; }
    void setStatus(Status status) { this->status_ = status; }

    bool operator==(const Ticket &other) const {
        return number_ == other.number_;
    }

    bool operator<(const Ticket &other) const {
        return number_ < other.getNumber();
    }

private:
    int number_;
    std::string name_;
    Status status_;
};

// linking label and color
const std::pair<std::string, QColor> status_data[] = {
    {"Не выучен", QColor(128, 128, 128)},
    {"Повторить", QColor(224, 206, 4)},
    {"Выучен", QColor(39, 168, 23)}
};

// hash for set
namespace std {
    template<>
    struct hash<Ticket> {
        std::size_t operator()(const Ticket &ticket) const {
            return hash<int>()(ticket.getNumber());
        }
    };
}
#endif //TICKET_H
