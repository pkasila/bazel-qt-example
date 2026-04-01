#include "Raycaster.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Raycaster raycaster;
    raycaster.setWindowTitle("Ваня когда хата?");
    raycaster.show();
    return app.exec();
}
