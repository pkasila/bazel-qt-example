#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Language Learning Lab");
    app.setOrganizationName("OpenAI-Lab");

    MainWindow window;
    window.show();

    return app.exec();
}
