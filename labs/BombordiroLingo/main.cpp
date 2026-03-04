#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyleSheet(R"(
        * {
            font-family: 'Arial';
            font-size: 14px;
        }
        QWidget {
            background-color: #f5f5f5;
            color: #333333;
        }
        QLabel {
            color: #333333;
            font-size: 16px;
        }
        QTextEdit, QRadioButton {
            background-color: white;
            color: #333333;
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 8px;
        }
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
    )");

    MainWindow w;
    w.show();
    return a.exec();
}
