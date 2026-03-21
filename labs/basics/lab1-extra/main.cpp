#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Кто ты по дате рождения");
    window.resize(800, 500);
    window.show();

    return app.exec();
}