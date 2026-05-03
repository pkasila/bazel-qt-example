#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Включение сглаживания шрифтов и UI
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    MainWindow w;
    w.show();
    return app.exec();
}
