#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.resize(800, 500);
    window.setWindowTitle("Трекер подготовки к экзамену");
    window.show();

    return app.exec();
}