#include <QApplication>

#include "main_window.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;

    mainWindow.show();
    mainWindow.setWindowTitle("Мои напоминания");
    mainWindow.resize(400, 500);

    return app.exec();
}
