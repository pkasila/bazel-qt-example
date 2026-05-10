#include "database/database_manager.h"
#include "interface/mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    DatabaseManager::instance().init();

    MainWindow w;
    w.show();
    return a.exec();
}
