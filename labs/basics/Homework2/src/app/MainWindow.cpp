#include "app/MainWindow.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "app/DrawWidget.h"
#include "ui/Modes.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    SetupUi();
    SetupConnections();

    resize(1000, 700);
    setWindowTitle("Raycaster");
}

void MainWindow::SetupUi() {
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* root_layout = new QVBoxLayout(central);
    auto* top_layout = new QHBoxLayout();

    auto* label = new QLabel("Mode:", central);
    mode_combo_box_ = new QComboBox(central);
    mode_combo_box_->addItem("Light");
    mode_combo_box_->addItem("Polygons");
    mode_combo_box_->addItem("StaticLights");

    top_layout->addWidget(label);
    top_layout->addWidget(mode_combo_box_);
    top_layout->addStretch();

    draw_widget_ = new DrawWidget(central);

    root_layout->addLayout(top_layout);
    root_layout->addWidget(draw_widget_, 1);
}

void MainWindow::SetupConnections() {
    connect(mode_combo_box_, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (index == 0) {
            draw_widget_->SetMode(InteractionMode::Light);
        } else if (index == 1) {
            draw_widget_->SetMode(InteractionMode::Polygons);
        } else {
            draw_widget_->SetMode(InteractionMode::StaticLights);
        }
    });
}