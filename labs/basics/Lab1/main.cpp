#include "ticket_review_app.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setStyle("Fusion");
    
    TicketReviewApp window;
    window.show();
    
    return app.exec();
}