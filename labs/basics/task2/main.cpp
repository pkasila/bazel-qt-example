#include "MyWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyWindow window;
    window.setWindowTitle("Рисование по клеточкам");
    window.resize(1000, 700);
    window.show();
    return a.exec();
}
