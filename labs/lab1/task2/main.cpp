#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;  // NOLINT(misc-const-correctness)
    window.setWindowTitle("Однорукий бандит");
    window.show();

    return QApplication::exec();
}
