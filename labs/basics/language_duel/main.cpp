#include <QApplication>

#include "ui/main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("Language Duel");
    QApplication::setOrganizationName("Mikhail Zaitsau Lab");

    MainWindow window;
    window.show();

    return app.exec();
}
