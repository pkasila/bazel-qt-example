#pragma once

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QWidget>

#include "scene.h"

class Canvas : public QWidget {
  Q_OBJECT

 public:
  enum InteractionMode { kLight, kPolygons };

  explicit Canvas(QWidget* parent = nullptr);

  void switch_mode(InteractionMode m);

 protected:
  void paintEvent(QPaintEvent* ev) override;
  void mousePressEvent(QMouseEvent* ev) override;
  void mouseMoveEvent(QMouseEvent* ev) override;
  void keyPressEvent(QKeyEvent* ev) override;

 private:
  void sync_wall();
  void render_shadows(QPainter& p);
  void render_obstacles(QPainter& p);
  void render_lamp(QPainter& p);

  Scene scene_;
  InteractionMode mode_ = kLight;
  bool poly_open_ = false;

  static constexpr int kNumLamps = 7;
  static constexpr double kSpread = 5.0;
};
