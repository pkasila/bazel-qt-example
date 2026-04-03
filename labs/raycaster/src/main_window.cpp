//
// Created by blalex on 4/6/25.
//

#include "main_window.h"

#include <QBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , drawing_widget_(new DrawingWidget(&controller_, this))
    , mode_selector_(new QComboBox(this))
    , fps_label_(new QLabel("0 FPS", this)) {
    SetupWindow();
}

void MainWindow::SetupWindow() {
    setWindowTitle("Raycaster");
    resize(800, 600);

    auto* central_widget = new QWidget(this);
    auto* main_layout = new QVBoxLayout(central_widget);
    main_layout->setContentsMargins(5, 5, 5, 5);
    main_layout->setSpacing(0);

    auto* top_layout = new QHBoxLayout;
    top_layout->setContentsMargins(5, 5, 5, 5);
    top_layout->setSpacing(10);

    mode_selector_->addItems({"Light", "Polygon"});
    mode_selector_->setCurrentIndex(0);
    mode_selector_->setFixedHeight(25);  // фиксированная высота
    mode_selector_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    fps_label_->setFixedHeight(25);
    fps_label_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    top_layout->addWidget(mode_selector_, 0, Qt::AlignLeft);
    top_layout->addStretch();
    top_layout->addWidget(fps_label_, 0, Qt::AlignRight);

    auto* top_controls = new QWidget(this);
    top_controls->setLayout(top_layout);
    top_controls->setFixedHeight(35);

    main_layout->addWidget(top_controls, 0);
    main_layout->addWidget(drawing_widget_, 1);

    setCentralWidget(central_widget);

    connect(mode_selector_, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &MainWindow::ChangeMode);
    connect(drawing_widget_, &DrawingWidget::FPSUpdated,
            fps_label_, &QLabel::setText);
}
void MainWindow::ChangeMode(const int mode) {
    controller_.SetMode(mode);
}