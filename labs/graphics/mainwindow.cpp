#include "mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

namespace raycaster {

MainWindow::MainWindow() {
    auto* central = new QWidget();
    auto* layout = new QVBoxLayout(central);

    auto* controls = new QHBoxLayout();
    mode_combo = new QComboBox();
    mode_combo->addItems({"Debug", "Prod"});

    sub_mode_combo = new QComboBox();
    sub_mode_combo->addItems({"Polygons", "Light"});

    connect(sub_mode_combo, &QComboBox::currentTextChanged, [this](const QString& text) {
        if (text == "Polygons") {
            render_widget->SetMode(Mode::Polygons);
        } else {
            render_widget->SetMode(Mode::Light);
        }
    });

    auto* clear_btn = new QPushButton("Clear Polygon");

    controls->addWidget(mode_combo);
    controls->addWidget(sub_mode_combo);
    controls->addWidget(clear_btn);
    controls->addStretch();

    render_widget = new RenderWidget();

    layout->addLayout(controls);
    layout->addWidget(render_widget);
    setCentralWidget(central);

    mode_combo->setCurrentText("Debug");

    connect(clear_btn, &QPushButton::clicked, [this]() { render_widget->ClearAll(); });

    connect(mode_combo, &QComboBox::currentTextChanged, [this](const QString& text) {
        bool is_debug = (text == "Debug");
        sub_mode_combo->setEnabled(is_debug);
        render_widget->setEnabled(is_debug);
    });

//     connect(mode_combo, &QComboBox::currentTextChanged, [this](const QString& text) {
//         bool is_debug = (text == "Debug");
//         sub_mode_combo->setEnabled(is_debug);
//         // Прокидываем состояние в виджет
//         render_widget->SetAppState(is_debug ? AppState::Debug : AppState::Prod);
//     });
    }

}  // namespace raycaster