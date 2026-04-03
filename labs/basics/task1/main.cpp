#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.resize(1080, 720);
    window.setMaximumHeight(720);
    window.setMaximumWidth(1080);
    window.setMinimumHeight(200);
    window.setMinimumWidth(400);
    window.setWindowTitle("Session checklist");

    window.show();
    return app.exec();
}
