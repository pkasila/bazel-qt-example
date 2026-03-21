#include <QApplication>
#include <QPushButton>

#include "MainWindow.h"

int main(int argc, char *argv[]) {
    // qputenv("QT_LOGGING_RULES", "qt.multimedia* = true");
    QApplication a(argc, argv);
    QString dbPath = (argc > 1) ? argv[1] : "mydata.db";
    MainWindow w(dbPath);
    w.show();
    return QApplication::exec();
}