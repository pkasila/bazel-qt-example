#include "TicketApp.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    TicketApp window;
    window.show();
    return app.exec();
}
