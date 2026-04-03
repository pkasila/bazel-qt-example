#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    QFont font = a.font();
    font.setPointSize(28);
    a.setFont(font);

    MainWindow w;
    w.resize(1200, 900);
    w.show();
    return a.exec();
}
