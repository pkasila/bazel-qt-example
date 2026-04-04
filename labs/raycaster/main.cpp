#include "labs/raycaster/ui/main_window.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    raycaster::MainWindow window;
    window.show();

    return QApplication::exec();
}
