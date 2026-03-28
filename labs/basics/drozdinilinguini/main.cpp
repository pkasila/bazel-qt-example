#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    TranslateMode::loadTasksFromMemory();
    GrammarMode::loadTasksFromMemory();

    MainWindow window;
    window.show();
    return app.exec();
}