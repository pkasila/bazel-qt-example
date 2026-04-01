#include "ticket.h"

Ticket::Ticket() {
}

void Ticket::setSatus(int st) {
    status = st;
}
int Ticket::getStatus() {
    return status;
}
void Ticket::setNum(int num) {
    number = num;
}
int Ticket::getNum() {
    return number;
}
void Ticket::setName(QString name) {
    this->name = name;
}
QString Ticket::getName() {
    return name;
}
