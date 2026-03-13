#include "viewwidget.h"
QuestionWidget::QuestionWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    next_question = new QPushButton("Случайный вопрос");
    prev_question = new QPushButton("Предыдущий вопрос");

    layout->addWidget(next_question);
    layout->addWidget(prev_question);

    connect(next_question, &QPushButton::clicked, this, &QuestionWidget::handleNext);
    connect(prev_question, &QPushButton::clicked, this, &QuestionWidget::handlePrev);
}
void QuestionWidget::addToHistory(Ticket* ticket) {
    if (isPrev) return;
    if (currentIndex < history.size() - 1) {
        history = history.mid(0, currentIndex + 1);
    }
    history.append(ticket);
    currentIndex = history.size() - 1;
    prev_question->setEnabled(currentIndex > 0);
    next_question->setEnabled(true);
}

void QuestionWidget::setTickets(const QList<Ticket*>& tickets) {
    this->tickets = tickets;
    history.clear();
    currentIndex = -1;
}

void QuestionWidget::handleNext() {
    if (tickets.isEmpty()) return;

    Ticket* newTicket = nullptr;
    int attempts = 0;
    const int maxAttempts = tickets.size() * 2;

    do {
        int idx = QRandomGenerator::global()->bounded(tickets.size());
        newTicket = tickets[idx];
        attempts++;
    } while (history.contains(newTicket) && attempts < maxAttempts);

    addToHistory(newTicket);
    emit ticketSelected(newTicket);
}

void QuestionWidget::handlePrev() {
    if (currentIndex <= 0 || history.isEmpty()) return;
    currentIndex--;
    isPrev = true;

    emit ticketSelected(history[currentIndex]);
    isPrev = false;

    prev_question->setEnabled(currentIndex > 0);
    next_question->setEnabled(true);
}
