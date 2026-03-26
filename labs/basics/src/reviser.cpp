#include <QVariant>
#include <random>
#include "reviser.hpp"
#include "ticket.hpp"

Reviser::Reviser() {
    gen = std::mt19937_64(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    connect(this, &Reviser::indexChanged, this, &Reviser::curNameChanged);
    connect(this, &Reviser::indexChanged, this, &Reviser::curStateChanged);
    connect(this, &Reviser::indexChanged, this, &Reviser::canNextChanged);
    addTicket();
}

void Reviser::previous() {
    if(history.empty()) return;
    index = history.back();
    history.pop_back();
    refresh();
}

void Reviser::next() {
    std::vector<int> variants;
    for(int i = 0; i < list.size(); i++) {
        if(i == index) continue;
        if(list.at(i)->property("state").toInt() != TICKET_VISITED) {
            variants.push_back(i);
        }
    }
    if(!variants.empty()) {
        history.push_back(index);
        std::uniform_int_distribution<int> dist(0, static_cast<int>(variants.size()) - 1);
        index = variants.at(dist(gen));
        refresh();
    }
}

void Reviser::addTicket() {
    newTicketNumber++;
    Ticket* ticket = new Ticket("Новый билет " + QString::number(newTicketNumber), TICKET_UNVISITED);
    connect(ticket, &Ticket::stateChanged, this, &Reviser::refresh);

    if(!list.empty()) {
        history.push_back(index);
    }
    list.push_back(ticket);
    index = static_cast<int>(list.size()) - 1;
    refresh();
}

void Reviser::removeTicket() {
    list.remove(index);
    index = std::max(0, index - 1);
    history.clear();
    refresh();
}

QString Reviser::getCurName() { return list.at(index)->property("name").toString(); }

void Reviser::setCurName(QString name) {
    list.at(index)->setProperty("name", name);
    refresh();
}

int Reviser::getCurState() { return list.at(index)->property("state").toInt(); }

void Reviser::setCurState(int state) {
    list.at(index)->setProperty("state", state);
    refresh();
}

double Reviser::getProgress() {
    int cnt = 0;
    for(QObject* object : list) {
        if(object->property("state").toInt() == TICKET_VISITED) {
            cnt++;
        }
    }
    return static_cast<double>(cnt) / static_cast<double>(list.size());
}

bool Reviser::canPrevious() { return !history.empty(); }

bool Reviser::canNext() {
    for(int i = 0; i < list.size(); i++) {
        if(i == index) continue;
        if(list.at(i)->property("state").toInt() != TICKET_VISITED) {
            return true;
        }
    }
    return false;
}

bool Reviser::canRemove() { return list.size() >= 2; }

void Reviser::refresh() {
    emit listChanged();
    emit indexChanged();

    emit curNameChanged();
    emit curStateChanged();
    emit progressChanged();

    emit canPreviousChanged();
    emit canNextChanged();
    emit canRemoveChanged();
}
