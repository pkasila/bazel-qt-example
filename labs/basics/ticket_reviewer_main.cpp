#include "ticket_reviewer.h"

#include <QtWidgets/QApplication>

int main(int argc, char** argv) {
    QApplication a(argc, argv);
    TicketReviewer reviewer;
    reviewer.show();
    return QApplication::exec();
}
