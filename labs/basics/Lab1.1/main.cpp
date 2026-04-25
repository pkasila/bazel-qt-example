#include <QApplication>
#include "exam_reviewer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    ExamReviewer window;
    window.show();
    
    return app.exec();
}
