#include "exam_app.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    ExamApp w;
    w.setWindowTitle("Exam Preparation Tool");
    w.resize(1000, 650);
    w.show();
    return a.exec();
}