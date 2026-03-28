#include "mainwindow.h"
#include "infobox.h"
#include "ticketitem.h"
#include "ticketlist.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow wind;
    wind.resize(300, 300);
    wind.show();
    return a.exec();
}
