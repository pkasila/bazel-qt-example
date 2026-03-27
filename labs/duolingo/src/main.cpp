#include "../include/duolingo.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication const a(argc, argv);
    Duolingo w;
    w.resize(800, 600);
    w.setWindowTitle("Duolingo Español");
    w.show();
    return QApplication::exec();
}