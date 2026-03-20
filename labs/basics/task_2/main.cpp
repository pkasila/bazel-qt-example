#include "text_editor.h"

#include  <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    TextEditor text_editor;
    text_editor.show();
    return app.exec();
}