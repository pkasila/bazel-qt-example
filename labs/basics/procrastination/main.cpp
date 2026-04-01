#include "procrastination.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.setWindowTitle("Прокрастинация");
    w.resize(600, 400);
    w.show();
    return QApplication::exec();
}
