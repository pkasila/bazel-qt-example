#ifndef RAYCASTER_WIDGET_H
#define RAYCASTER_WIDGET_H

#include <QMouseEvent>
#include <QPaintEvent>
#include <QWidget>

#include "controller.h"

class RaycasterWidget : public QWidget {
  Q_OBJECT

 public:
  enum Mode { LightMode, PolygonMode };

  explicit RaycasterWidget(QWidget* parent = nullptr);

  void SetMode(Mode mode);
  [[nodiscard]] Mode GetMode() const;

 protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

 private:
  void DrawPolygons(QPainter& painter);
  void DrawLightSource(QPainter& painter);
  void DrawLightArea(QPainter& painter);
  void EnsureBoundaryPolygon();

  Controller controller_;
  Mode current_mode_ = LightMode;
  bool drawing_polygon_ = false;
  int penumbra_count_ = 5;
  double penumbra_spread_ = 4.0;
};

#endif
