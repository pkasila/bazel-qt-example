#include <QApplication>

#include "main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QWidget* window = createMainWindow();
    window->show();

    const int exitCode = app.exec();
    delete window;
    return exitCode;
}
