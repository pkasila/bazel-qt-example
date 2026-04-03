//
// Created by blalex on 4/6/25.
//
// #include "main_window.h"

#include <QApplication>

#include "src/main_window.h"


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return QApplication::exec();
}
