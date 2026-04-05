// #ifndef MAINWINDOW_H
// #define MAINWINDOW_H

// #include <QMainWindow>
// #include <QComboBox>
// #include <QPushButton>
// #include <QVBoxLayout>
// #include "render_widget.h"

// namespace raycaster {

// class MainWindow : public QMainWindow {
//     Q_OBJECT
// public:
//     MainWindow() {
//         auto* central = new QWidget();
//         auto* layout = new QVBoxLayout(central);

//         // Верхняя панель управления
//         auto* controls = new QHBoxLayout();
        
//         mode_combo = new QComboBox();
//         mode_combo->addItems({"Debug", "Prod"});
        
//         sub_mode_combo = new QComboBox();
//         sub_mode_combo->addItems({"Polygons", "Ray"});
        
//         auto* clear_btn = new QPushButton("Clear Polygon");

//         controls->addWidget(mode_combo);
//         controls->addWidget(sub_mode_combo);
//         controls->addWidget(clear_btn);
//         controls->addStretch();

//         render_widget = new RenderWidget();
        
//         layout->addLayout(controls);
//         layout->addWidget(render_widget);
//         setCentralWidget(central);

//         // Константа режима по умолчанию
//         const QString default_mode = "Debug";
//         mode_combo->setCurrentText(default_mode);

//         // Коннекты
//         connect(sub_mode_combo, &QComboBox::currentTextChanged, [this](const QString& text){
//             render_widget->SetSubMode(text == "Polygons" ? DebugSubMode::Polygons : DebugSubMode::Ray);
//         });

//         connect(clear_btn, &QPushButton::clicked, [this](){
//             render_widget->ClearPolygon();
//         });

//         // Блокировка интерфейса в режиме Prod
//         connect(mode_combo, &QComboBox::currentTextChanged, [this](const QString& text){
//             sub_mode_combo->setEnabled(text == "Debug");
//             render_widget->setEnabled(text == "Debug");
//         });
//     }

// private:
//     QComboBox* mode_combo;
//     QComboBox* sub_mode_combo;
//     RenderWidget* render_widget;
// };

// }
// #endif



#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include "render_widget.h"

namespace raycaster {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();

private:
    QComboBox* mode_combo;
    QComboBox* sub_mode_combo;
    RenderWidget* render_widget;
};

} // namespace raycaster

#endif