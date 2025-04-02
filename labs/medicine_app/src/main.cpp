#include "MedicineApp.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MedicineApp w;
    w.show();
    return a.exec();
}
