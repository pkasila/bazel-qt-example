#include <QApplication>

#include "app_window.h"

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  AppWindow win;
  win.resize(960, 680);
  win.show();

  return app.exec();
}
