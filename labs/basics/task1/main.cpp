#include <QApplication>
#include "ticketapp.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    TicketApp window;
    window.show();
    return app.exec();
}
