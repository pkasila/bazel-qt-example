#ifndef TICKET_H
#define TICKET_H
#include <QObject>
#include <QString>

class Ticket : public QObject {
    Q_OBJECT
public:
    enum Status : uint8_t {
        DEFAULT, YELLOW, GREEN
    };
    explicit Ticket(int ticket_number) : number_(ticket_number) {
        name_ = QString("Билет %1").arg(ticket_number);
    }
    [[nodiscard]] QString GetName() const {
        return name_;
    }
    [[nodiscard]] Status GetStatus() const {
        return status_;
    }
    [[nodiscard]] int GetNumber() const {
        return number_;
    }
    [[nodiscard]] int GetIndex() const {
        return number_ - 1;
    }
    void SetName(const QString& name) {
        if (name_ == name) { return; }
        name_ = name;
        emit NameChanged(name);
    }
    void SetStatus(Status status) {
        if (status_ == status) { return; }
        const auto tmp = status_;
        status_ = status;
        emit StatusChanged(status, tmp);
    }

signals:
    void NameChanged(const QString& name);
    void StatusChanged(Ticket::Status status, Ticket::Status prev_status);

private:
    int number_;
    QString name_;
    Status status_ = DEFAULT;
};

#endif //TICKET_H
