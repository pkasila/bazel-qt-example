#include <QApplication>
#include "widget.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    RaycasterWidget widget;
    widget.setWindowTitle("2D Raycaster - Lab 2");
    widget.show();
    return app.exec();
}