#include "main_window.h"

#include <QApplication>

//NOLINTBEGIN
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

//NOLINTEND