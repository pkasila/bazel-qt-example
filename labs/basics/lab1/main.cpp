#include <QApplication>
#include "ticket_review_app.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  TicketReviewApp window;
  window.show();

  return app.exec();
}
