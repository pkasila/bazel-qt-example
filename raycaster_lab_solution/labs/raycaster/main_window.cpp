#include "labs/raycaster/main_window.h"

#include "labs/raycaster/drawing_widget.h"

#include <QtCore/QString>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace raycaster {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    auto* central_widget = new QWidget(this);
    auto* root_layout = new QVBoxLayout(central_widget);
    root_layout->setContentsMargins(12, 12, 12, 12);
    root_layout->setSpacing(10);

    auto* controls_layout = new QHBoxLayout();
    controls_layout->setSpacing(10);

    auto* mode_title = new QLabel("Mode:", central_widget);
    controls_layout->addWidget(mode_title);

    mode_combo_box_ = new QComboBox(central_widget);
    mode_combo_box_->addItem("light");
    mode_combo_box_->addItem("polygons");
    controls_layout->addWidget(mode_combo_box_);

    help_label_ = new QLabel(
        "Light: move mouse to move the light cluster.  "
        "Polygons: LMB starts/adds vertices, RMB finishes current polygon.",
        central_widget);
    help_label_->setWordWrap(true);
    controls_layout->addWidget(help_label_, 1);

    root_layout->addLayout(controls_layout);

    auto* separator = new QFrame(central_widget);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    root_layout->addWidget(separator);

    drawing_widget_ = new DrawingWidget(central_widget);
    drawing_widget_->setMinimumSize(900, 600);
    root_layout->addWidget(drawing_widget_, 1);

    setCentralWidget(central_widget);
    resize(1100, 760);
    setWindowTitle("Lab 2: Raycaster");

    QObject::connect(mode_combo_box_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        [this](int index) {
            if (index == 0) {
                drawing_widget_->SetMode(DrawingWidget::Mode::kLight);
            } else {
                drawing_widget_->SetMode(DrawingWidget::Mode::kPolygons);
            }
        });
}

}  // namespace raycaster
