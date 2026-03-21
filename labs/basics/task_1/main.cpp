#include <QApplication>

#include "custom_widgets.h"
#include "main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(resources);
    StartupMenu startupMenu;
    MainWindow mainWindow;
    QObject::connect(&startupMenu, &StartupMenu::startClicked, [&mainWindow, &startupMenu]() {
        mainWindow.show();
        startupMenu.close();
    });
    startupMenu.show();
    return app.exec();
}