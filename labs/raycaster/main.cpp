#include <QApplication>
#include "raycaster.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    Raycaster window;
    window.setWindowTitle("2D Raycaster");
    window.show();
    
    return app.exec();
}