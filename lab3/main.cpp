#include "main_window.h"

#include <QApplication>
#include <QFont>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setFont(QFont("Segoe UI", 10));
    MainWindow w;
    w.show();
    return app.exec();
}