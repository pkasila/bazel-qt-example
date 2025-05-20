#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    qputenv("QT_FFMPEG_NO_VAAPI", "1");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    //Grammar* menu = new Grammar();
    //menu->show();
    return a.exec();
}
