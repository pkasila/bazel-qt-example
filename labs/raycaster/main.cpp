#include <QApplication>
#include "mainwindow.h"
#include "board.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainwindow w;
    w.resize(960, 640);
    w.setWindowTitle("рейкастер");
    w.show();

    return QApplication::exec();
}
// можно менять размер экрана и оно даже будет хорошо работать
// впринципе, сделано всё
