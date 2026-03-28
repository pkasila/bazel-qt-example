#ifndef TICKETITEM_H
#define TICKETITEM_H

#include <QObject>
#include <QString>
#include <QListWidgetItem>

class TicketItem : public QObject, public QListWidgetItem
{
    Q_OBJECT
public:
    enum TicketStatus { Default, NeedReview, Reviewed };

    explicit TicketItem(int number, QObject *parent = nullptr);

    void updateItemStatus();
    void updateColor();

    int getNumber() const { return number; }
    QString getName() const { return text(); }
    int getStatus() const { return status; }
    void setName(const QString &name) { setText(name); }
    void setStatus(int newStatus) {
        status = static_cast<TicketStatus>(newStatus);
        updateColor();
        emit progressUpdated();
    }

signals:
    void progressUpdated();

private:
    TicketStatus status;
    int number;
};

#endif // TICKETITEM_H
