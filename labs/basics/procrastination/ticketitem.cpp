#include "ticketitem.h"

TicketItem::TicketItem(int num, QObject *parent)
    : QObject(parent), QListWidgetItem("Билет " + QString::number(num)), number(num), status(Default)
{
    updateColor();
}

void TicketItem::updateItemStatus() {
    if (status == Default) {
        status = Reviewed;
    } else if (status == NeedReview) {
        status = Reviewed;
    } else {
        status = NeedReview;
    }
    updateColor();
    emit progressUpdated();
}

void TicketItem::updateColor() {
    if (status == Default) {
        setBackground(Qt::gray);
    } else if (status == NeedReview) {
        setBackground(Qt::yellow);
    } else if (status == Reviewed) {
        setBackground(Qt::green);
    }
}
