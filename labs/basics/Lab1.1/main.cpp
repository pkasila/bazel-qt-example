#include <QApplication>
#include "exam_app.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ExamApp w;
    w.setWindowTitle("Exam Preparation Tool");
    w.resize(800, 500);
    w.show();
    return a.exec();
}