#include <QApplication>

#include "ocean_control_app.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName(QStringLiteral("free_swim_lab"));

  OceanControlApp window;
  window.show();

  return QApplication::exec();
}
