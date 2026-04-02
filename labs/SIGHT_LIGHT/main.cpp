#include "mainwindow.h"
#include <QApplication>
#include <QCursor>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QPoint startPos(300, 300);

    QCursor::setPos(startPos);


    w.show();
    return a.exec();
}
