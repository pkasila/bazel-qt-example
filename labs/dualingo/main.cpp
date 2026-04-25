#include "lang_app.h"
#include <QtWidgets/QApplication>
#include <QtCore/QDir> // <--- ДОБАВИЛИ ДЛЯ РАБОТЫ С ПУТЯМИ

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Получаем точный абсолютный путь до картинки в песочнице Bazel
    QString catPath = QDir::currentPath() + "/labs/dualingo/cat.png";
    
    // Кошачий QSS стиль (MeowLingo)
    // Используем QString::arg() чтобы безопасно подставить путь
    QString style = QString(
        "#MeowWindow { "
        "   background-color: #fff6e5; "
        "   background-image: url('%1'); " /* Сюда подставится точный путь */
        "   background-position: bottom right; " 
        "   background-repeat: no-repeat; " 
        "}"
        
        "QPushButton { "
        "   background-color: #ff9f43; " 
        "   color: white; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "   border-radius: 12px; "
        "   padding: 10px; "
        "}"
        "QPushButton:hover { background-color: #ee5253; }" 
        "QLineEdit, QTextEdit { "
        "   padding: 10px; "
        "   border: 2px solid #ff9f43; "
        "   border-radius: 12px; "
        "   font-size: 15px; "
        "   background-color: rgba(255, 255, 255, 0.9);"
        "}"
        "QLabel { color: #2d3436; font-size: 16px; }"
        "QProgressBar { "
        "   text-align: center; "
        "   font-weight: bold;"
        "   color: #2d3436;"
        "   border-radius: 10px; "
        "   border: 2px solid #ff9f43; "
        "   background-color: white;"
        "}"
        "QProgressBar::chunk { "
        "   background-color: #ff9f43; "
        "   border-radius: 8px; "
        "}"
        "QRadioButton { font-size: 16px; spacing: 10px; }"
        "QRadioButton::indicator { width: 18px; height: 18px; }"
    ).arg(catPath); // Вставляем путь вместо %1

    app.setStyleSheet(style);

    LangApp window;
    window.setObjectName("MeowWindow"); 
    window.setWindowTitle("MeowLingo 🐈 - Учи английский с котиками!");
    window.show();
    
    return app.exec();
}