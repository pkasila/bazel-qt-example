#include "labs/raycaster/main_window.h"

#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    raycaster::MainWindow window;
    window.show();
    return application.exec();
}
