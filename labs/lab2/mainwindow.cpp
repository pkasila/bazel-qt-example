#include "mainwindow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  SetupUi();
}

void MainWindow::SetupUi() {
  QWidget* central = new QWidget(this);
  setCentralWidget(central);

  QVBoxLayout* layout = new QVBoxLayout(central);
  layout->setContentsMargins(10, 10, 10, 10);
  layout->setSpacing(8);

  QHBoxLayout* toolbar = new QHBoxLayout();
  toolbar->setSpacing(10);

  QLabel* mode_label = new QLabel("Режим:");
  mode_label->setStyleSheet("color: #ddd; font-weight: bold; font-size: 14px;");
  toolbar->addWidget(mode_label);

  mode_combo_ = new QComboBox();
  mode_combo_->addItem("light");
  mode_combo_->addItem("polygons");
  mode_combo_->setStyleSheet(
      "QComboBox { background: #3a3a3a; color: #eee; border: 1px solid #555; "
      "border-radius: 4px; padding: 4px 8px; font-size: 13px; }"
      "QComboBox::drop-down { border: none; }"
      "QComboBox QAbstractItemView { background: #3a3a3a; color: #eee; "
      "selection-background-color: #555; }");
  toolbar->addWidget(mode_combo_);
  toolbar->addStretch();

  QLabel* hint_label = new QLabel();
  hint_label->setStyleSheet("color: #999; font-size: 12px;");
  hint_label->setText(
      "light: двигайте мышь | polygons: ЛКМ = вершина, ПКМ = завершить");
  toolbar->addWidget(hint_label);

  layout->addLayout(toolbar);

  raycaster_ = new RaycasterWidget();
  raycaster_->setStyleSheet("border: 1px solid #444; border-radius: 4px;");
  layout->addWidget(raycaster_, 1);

  central->setStyleSheet("background-color: #2a2a2a;");

  connect(mode_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &MainWindow::OnModeChanged);

  setWindowTitle("Lab 2: 2D Raycaster");
}

void MainWindow::OnModeChanged(int index) {
  if (index == 0) {
    raycaster_->SetMode(RaycasterWidget::LightMode);
  } else {
    raycaster_->SetMode(RaycasterWidget::PolygonMode);
  }
}
