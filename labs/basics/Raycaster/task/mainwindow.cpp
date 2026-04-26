#include "mainwindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Polygons and Light");
    resize(800, 600);

    auto* central_widget = new QWidget(this);
    setCentralWidget(central_widget);

    auto* layout = new QVBoxLayout(central_widget);
    layout->setContentsMargins(0, 0, 0, 0);

    mode_combo_box_ = new QComboBox(this);
    mode_combo_box_->addItem("Polygons Mode");
    mode_combo_box_->addItem("Light Mode");
    mode_combo_box_->setFixedWidth(200);
    layout->addWidget(mode_combo_box_, 0, Qt::AlignTop | Qt::AlignLeft);

    drawing_widget_ = new DrawingWidget(this);
    drawing_widget_->setFocusPolicy(Qt::StrongFocus);
    layout->addWidget(drawing_widget_, 1);

    connect(
        mode_combo_box_, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
            drawing_widget_->SetMode(index == 0 ? Mode::kPolygons : Mode::kLight);
        });

    drawing_widget_->SetDrawingStateCallback([this](bool is_drawing) {
        mode_combo_box_->setEnabled(!is_drawing);
    });
}