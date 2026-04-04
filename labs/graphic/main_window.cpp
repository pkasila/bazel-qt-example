#include "main_window.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "canvas_widget.h"

MainWindow::MainWindow() : canvas_(new CanvasWidget(this)), mode_box_(new QComboBox(this)), help_label_(new QLabel(this)) {
    auto* central = new QWidget(this);
    auto* root_layout = new QVBoxLayout(central);
    auto* top_layout = new QHBoxLayout();

    auto* mode_title = new QLabel("Mode:", central);
    mode_box_->addItem("light");
    mode_box_->addItem("polygons");

    help_label_->setText("Light: move mouse. Polygons: LMB add vertex, RMB finish polygon.");

    top_layout->addWidget(mode_title);
    top_layout->addWidget(mode_box_);
    top_layout->addSpacing(16);
    top_layout->addWidget(help_label_, 1);

    root_layout->addLayout(top_layout);
    root_layout->addWidget(canvas_, 1);

    setCentralWidget(central);
    setWindowTitle("Raycaster Lab");
    resize(1100, 800);

    connect(mode_box_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index == 0) {
            canvas_->SetMode(CanvasWidget::Mode::Light);
        } else {
            canvas_->SetMode(CanvasWidget::Mode::Polygons);
        }
    });
}
