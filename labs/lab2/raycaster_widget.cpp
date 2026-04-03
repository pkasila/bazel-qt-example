#include "raycaster_widget.h"

#include <QPainter>
#include <QPainterPath>
#include <cmath>

RaycasterWidget::RaycasterWidget(QWidget* parent) : QWidget(parent) {
  setMouseTracking(true);
  setMinimumSize(600, 400);
  controller_.SetLightSource(QPointF(300, 200));
}

void RaycasterWidget::SetMode(Mode mode) {
  current_mode_ = mode;
  if (mode == LightMode) {
    drawing_polygon_ = false;
  }
  update();
}

RaycasterWidget::Mode RaycasterWidget::GetMode() const {
  return current_mode_;
}

void RaycasterWidget::EnsureBoundaryPolygon() {
  double w = width();
  double h = height();
  Polygon boundary({QPointF(0, 0), QPointF(w, 0), QPointF(w, h), QPointF(0, h)});

  if (controller_.GetPolygons().empty()) {
    controller_.AddPolygon(boundary);
  } else {
    controller_.SetBoundaryPolygon(boundary);
  }
}

void RaycasterWidget::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.fillRect(rect(), QColor(30, 30, 30));

  EnsureBoundaryPolygon();

  DrawLightArea(painter);
  DrawPolygons(painter);
  DrawLightSource(painter);
}

void RaycasterWidget::DrawPolygons(QPainter& painter) {
  const auto& polygons = controller_.GetPolygons();

  for (size_t i = 1; i < polygons.size(); ++i) {
    const auto& verts = polygons[i].GetVertices();
    if (verts.size() < 2) {
      continue;
    }

    painter.setPen(QPen(QColor(100, 180, 255), 2));
    painter.setBrush(QColor(60, 100, 140, 80));

    QPainterPath path;
    path.moveTo(verts[0]);
    for (size_t j = 1; j < verts.size(); ++j) {
      path.lineTo(verts[j]);
    }
    path.closeSubpath();
    painter.drawPath(path);
  }
}

void RaycasterWidget::DrawLightSource(QPainter& painter) {
  QPointF ls = controller_.GetLightSource();

  painter.setPen(Qt::NoPen);
  QRadialGradient glow(ls, 15);
  glow.setColorAt(0, QColor(255, 220, 50, 255));
  glow.setColorAt(1, QColor(255, 220, 50, 0));
  painter.setBrush(glow);
  painter.drawEllipse(ls, 15, 15);

  painter.setBrush(QColor(255, 240, 100));
  painter.drawEllipse(ls, 5, 5);
}

void RaycasterWidget::DrawLightArea(QPainter& painter) {
  if (controller_.GetPolygons().empty()) {
    return;
  }

  struct LightInstance {
    QPointF offset;
    int alpha;
  };

  std::vector<LightInstance> lights;
  int base_alpha = 180 / penumbra_count_;

  for (int i = 0; i < penumbra_count_; ++i) {
    double angle = 2.0 * M_PI * i / penumbra_count_;
    double ox = penumbra_spread_ * std::cos(angle);
    double oy = penumbra_spread_ * std::sin(angle);
    lights.push_back({QPointF(ox, oy), base_alpha});
  }

  QPointF original_light = controller_.GetLightSource();

  for (const auto& light : lights) {
    QPointF shifted = original_light + light.offset;
    controller_.SetLightSource(shifted);

    Polygon light_poly = controller_.CreateLightArea();
    const auto& verts = light_poly.GetVertices();

    if (verts.size() < 3) {
      continue;
    }

    QPainterPath path;
    path.moveTo(verts[0]);
    for (size_t j = 1; j < verts.size(); ++j) {
      path.lineTo(verts[j]);
    }
    path.closeSubpath();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 230, 80, light.alpha));
    painter.drawPath(path);
  }

  controller_.SetLightSource(original_light);
}

void RaycasterWidget::mousePressEvent(QMouseEvent* event) {
  if (current_mode_ == PolygonMode) {
    if (event->button() == Qt::LeftButton) {
      QPointF pos = event->pos();
      if (!drawing_polygon_) {
        drawing_polygon_ = true;
        Polygon new_poly;
        new_poly.AddVertex(pos);
        new_poly.AddVertex(pos);
        controller_.AddPolygon(new_poly);
      } else {
        controller_.AddVertexToLastPolygon(pos);
      }
      update();
    } else if (event->button() == Qt::RightButton) {
      drawing_polygon_ = false;
      update();
    }
  }
}

void RaycasterWidget::mouseMoveEvent(QMouseEvent* event) {
  QPointF pos = event->pos();

  if (current_mode_ == LightMode) {
    controller_.SetLightSource(pos);
    update();
  } else if (current_mode_ == PolygonMode && drawing_polygon_) {
    controller_.UpdateLastPolygon(pos);
    update();
  }
}
