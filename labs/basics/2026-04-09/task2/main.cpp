#include <QApplication>

#include "mainwindow.h"

auto main(int argc, char* argv[]) -> int {
    QApplication application(argc, argv);

    MainWindow window;
    window.show();

    return QApplication::exec();
}