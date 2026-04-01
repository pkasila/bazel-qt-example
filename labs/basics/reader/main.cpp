#include "src/hdrs/RanobeReader.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication const app(argc, argv);
    RanobeReader w;
    w.setWindowTitle("Ранобе либ оффлайн");
    w.resize(600, 400);
    w.show();
    return QApplication::exec();
}
