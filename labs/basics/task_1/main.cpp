#include <qapplication.h>
#include <QtCore/qfile.h>

#include "main_window.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.resize(400, 670);
    mainWindow.show();

    return app.exec();
}
