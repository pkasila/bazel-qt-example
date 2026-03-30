#include <QtWidgets/QApplication>
#include <QtWidgets>

#include "Controller.h"


int main(int argc, char** argv) {
    QApplication a(argc, argv);

    Controller controller;
    controller.show();
    controller.setWindowTitle("Мои напоминания");
    controller.resize(500, 500);
    return a.exec();
}
