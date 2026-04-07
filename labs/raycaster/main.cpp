#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication const a(argc, argv);
        MainWindow w;
        w.resize(1000, 750);
        w.show();
    return QApplication::exec();
}
