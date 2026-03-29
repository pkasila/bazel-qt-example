#include "MyWindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyWindow window;
    window.setWindowTitle("Прокрастинация");
    window.resize(800, 400);
    window.show();
    return a.exec();
}
