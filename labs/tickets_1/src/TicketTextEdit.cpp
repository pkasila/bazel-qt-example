#include "TicketTextEdit.h"

TicketTextEdit::TicketTextEdit(QWidget *parent) :
    QTextEdit(parent) {}

void TicketTextEdit::focusOutEvent(QFocusEvent *event) {
    QTextEdit::focusOutEvent(event);
    emit editingFinished(toHtml());
}
