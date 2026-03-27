#include "app/Application.h"
#include "ui/MainWindow.h"
#include <QApplication> // Убедитесь, что QApplication включен

int main(int argc, char *argv[]) {
    Application app(argc, argv);

    // TODO: Initialize DatabaseManager, SettingsManager etc.
    // DatabaseManager::instance().init("path/to/your/database.sqlite");

    MainWindow w;
    w.show();

    return app.exec();
}