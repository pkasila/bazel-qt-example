#include "infobox.h"
#include <QVBoxLayout>

InfoBox::InfoBox(QWidget *parent) : QGroupBox(parent), currenttick(nullptr) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    number = new QLabel("Билет ", this);
    layout->addWidget(number);

    name_edit = new QLineEdit(this);
    layout->addWidget(name_edit);

    status = new QComboBox(this);
    status->addItem("Не изучен", TicketItem::Default);
    status->addItem("Нужно повторить", TicketItem::NeedReview);
    status->addItem("Выучен", TicketItem::Reviewed);
    layout->addWidget(status);

    connect(name_edit, &QLineEdit::editingFinished, this, &InfoBox::change_the_name);
    connect(status, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &InfoBox::change_the_status);
}

void InfoBox::setTicket(TicketItem *ticket) {
    if (!ticket) return;
    currenttick = ticket;
    number->setText("Билет " + QString::number(ticket->getNumber()));
    name_edit->setText(ticket->getName());
    status->setCurrentIndex(ticket->getStatus());
}

void InfoBox::change_the_name() {
    if (currenttick) {
        currenttick->setName(name_edit->text());
    }
}

void InfoBox::change_the_status(int index) {
    if (currenttick) {
        currenttick->setStatus(index);
    }
}
