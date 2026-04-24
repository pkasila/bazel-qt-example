#include "mainwindow.h"
#include <QApplication>

int main(int argc, char **argv) {
    QApplication app (argc, argv);
    MainWindow w;
    w.setWindowTitle("Tickets tracker");
    w.resize(800, 500);
    w.show();
    
    return app.exec(); 
}