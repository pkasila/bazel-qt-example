#include <QApplication>
#include "launcher.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    Launcher l;
    l.show();
    
    return a.exec();
}