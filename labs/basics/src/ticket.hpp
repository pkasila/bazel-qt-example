#ifndef TICKET_HPP
#define TICKET_HPP

#include <QObject>

enum { TICKET_UNVISITED, TICKET_REVISE, TICKET_VISITED };

class Ticket : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER name NOTIFY nameChanged)
    Q_PROPERTY(int state MEMBER state NOTIFY stateChanged)

public:
    Ticket(QString name, int state);

signals:
    void nameChanged();
    void stateChanged();

private:
    QString name;
    int state;
};

#endif // TICKET_HPP
