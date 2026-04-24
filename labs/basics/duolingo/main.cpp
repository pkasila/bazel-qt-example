#include <QApplication>
#include "mainwindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    MainWindow window;
    window.resize(600, 400);
    window.setWindowTitle("Qt Language Learner");
    window.show();

    return app.exec();
}