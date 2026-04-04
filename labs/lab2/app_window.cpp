#include "app_window.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

AppWindow::AppWindow(QWidget* parent) : QMainWindow(parent) {
  auto* central = new QWidget(this);
  setCentralWidget(central);

  auto* root = new QVBoxLayout(central);
  root->setContentsMargins(8, 8, 8, 8);
  root->setSpacing(6);

  auto* bar = new QHBoxLayout();
  bar->setSpacing(12);

  auto* title = new QLabel("Режим:");
  title->setStyleSheet("color:#ccc; font-size:13px;");
  bar->addWidget(title);

  rb_light_ = new QRadioButton("light");
  rb_light_->setChecked(true);
  rb_light_->setStyleSheet("QRadioButton{color:#bbb; font-size:13px;}");
  bar->addWidget(rb_light_);

  rb_poly_ = new QRadioButton("polygons");
  rb_poly_->setStyleSheet("QRadioButton{color:#bbb; font-size:13px;}");
  bar->addWidget(rb_poly_);

  bar->addStretch();

  auto* tip = new QLabel("WASD / мышь = свет | ЛКМ = вершина, ПКМ = готово");
  tip->setStyleSheet("color:#777; font-size:11px;");
  bar->addWidget(tip);

  root->addLayout(bar);

  canvas_ = new Canvas();
  root->addWidget(canvas_, 1);

  central->setStyleSheet("background:#1a1c22;");

  connect(rb_light_, &QRadioButton::toggled, this, &AppWindow::on_light_toggled);
  connect(rb_poly_, &QRadioButton::toggled, this, &AppWindow::on_poly_toggled);

  setWindowTitle("Raycaster");
}

void AppWindow::on_light_toggled(bool checked) {
  if (checked) canvas_->switch_mode(Canvas::kLight);
}

void AppWindow::on_poly_toggled(bool checked) {
  if (checked) canvas_->switch_mode(Canvas::kPolygons);
}
