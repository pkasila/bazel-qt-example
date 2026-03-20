#include <QApplication>

#include "greenhouse_control_app.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  GreenhouseControlApp window;
  window.show();
  return app.exec();
}
