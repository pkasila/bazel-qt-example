#ifndef TICKET_H
#define TICKET_H

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QButtonGroup>

class Ticket
{
public:
    Ticket();
    void setSatus(int st);
    int getStatus();
    void setNum(int num);
    int getNum();
    void setName(QString name);
    QString getName();
    Ticket(const Ticket&) = default;

private:
    int status; // 0 - gray, 1 - yellow, 2 - green
    int number;
    QString name;
    QString name_edit;
};

#endif // TICKET_H
