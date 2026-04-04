#pragma once

#include <QMainWindow>
#include <QRadioButton>

#include "canvas.h"

class AppWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit AppWindow(QWidget* parent = nullptr);

 private slots:
  void on_light_toggled(bool checked);
  void on_poly_toggled(bool checked);

 private:
  Canvas* canvas_;
  QRadioButton* rb_light_;
  QRadioButton* rb_poly_;
};
