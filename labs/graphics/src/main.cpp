#include "../include/raycaster_widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication const a(argc, argv);
    RaycasterWidget w;
    w.resize(800, 600);
    w.setWindowTitle("Raycaster");
    w.show();
    return QApplication::exec();
}
