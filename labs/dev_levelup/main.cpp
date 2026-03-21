#include "dev_levelup.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyleSheet(
        "QWidget { background-color: #2b2b2b; color: #efefef; font-family: 'Segoe UI', sans-serif; "
        "}"
        "QLineEdit, QSpinBox, QListWidget { background-color: #3b3c3d; border: 1px solid #555; "
        "padding: 5px; }"
        "QPushButton { background-color: #0d6efd; border-radius: 4px; padding: 8px; font-weight: "
        "bold; }"
        "QPushButton:hover { background-color: #0b5ed7; }"
        "QProgressBar { border: 1px solid #555; border-radius: 5px; text-align: center; }"
        "QProgressBar::chunk { background-color: #2ecc71; }");

    DevLevelUp window;
    window.show();
    return app.exec();
}