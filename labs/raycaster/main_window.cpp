#include "labs/raycaster/main_window.h"

#include <QComboBox>
#include <QVBoxLayout>
#include <QWidget>

#include "labs/raycaster/canvas_widget.h"

MainWindow::MainWindow()
    : mode_combo_(new QComboBox(this)), canvas_(new CanvasWidget(this)) {
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    mode_combo_->addItem("light");
    mode_combo_->addItem("polygons");

    connect(mode_combo_, &QComboBox::currentIndexChanged, this, [this](int index) {
        canvas_->SetMode(index == 0 ? InteractionMode::kLight : InteractionMode::kPolygons);
    });

    layout->addWidget(mode_combo_);
    layout->addWidget(canvas_, 1);

    setCentralWidget(central);
    resize(960, 720);
    setWindowTitle("Raycaster");
}
