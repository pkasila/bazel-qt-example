#include <QApplication>

#include "ticket_review_app.h"

int main(int argc, char* argv[]) {
  QApplication application(argc, argv);
  TicketReviewApp window;
  window.show();
  return application.exec();
}
