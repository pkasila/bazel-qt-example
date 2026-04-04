#include "raycaster.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Raycaster window;
    window.show();
    return app.exec();
}
