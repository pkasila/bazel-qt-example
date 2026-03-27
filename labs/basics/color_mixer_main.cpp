#include "color_mixer.h"

#include <QtWidgets/QApplication>

int main(int argc, char** argv) {
    QApplication a(argc, argv);
    ColorMixer mixer;
    mixer.show();
    return QApplication::exec();
}
