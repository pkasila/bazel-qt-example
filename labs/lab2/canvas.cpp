#include "canvas.h"

#include <QPainter>
#include <QPainterPath>
#include <cmath>

Canvas::Canvas(QWidget* parent) : QWidget(parent) {
  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);
  setMinimumSize(640, 480);
  scene_.set_lamp(QPointF(320, 240));
}

void Canvas::switch_mode(InteractionMode m) {
  mode_ = m;
  if (m == kLight) poly_open_ = false;
  update();
}

void Canvas::sync_wall() {
  double w = width(), h = height();
  Obstacle wall({QPointF(0, 0), QPointF(w, 0), QPointF(w, h), QPointF(0, h)});
  if (scene_.obstacles().empty())
    scene_.add_obstacle(std::move(wall));
  else
    scene_.set_wall(std::move(wall));
}

void Canvas::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  p.fillRect(rect(), QColor(20, 22, 28));

  sync_wall();
  render_shadows(p);
  render_obstacles(p);
  render_lamp(p);
}

void Canvas::render_obstacles(QPainter& p) {
  auto& obs = scene_.obstacles();
  for (size_t i = 1; i < obs.size(); ++i) {
    auto& pts = obs[i].points();
    if (pts.size() < 2) continue;

    QPainterPath shape;
    shape.moveTo(pts[0]);
    for (size_t j = 1; j < pts.size(); ++j) shape.lineTo(pts[j]);
    shape.closeSubpath();

    p.setPen(QPen(QColor(200, 120, 80), 2));
    p.setBrush(QColor(80, 50, 35, 120));
    p.drawPath(shape);
  }
}

void Canvas::render_lamp(QPainter& p) {
  QPointF pos = scene_.lamp();

  p.setPen(Qt::NoPen);
  QRadialGradient halo(pos, 20);
  halo.setColorAt(0, QColor(180, 200, 255, 200));
  halo.setColorAt(1, QColor(180, 200, 255, 0));
  p.setBrush(halo);
  p.drawEllipse(pos, 20, 20);

  p.setBrush(QColor(220, 235, 255));
  p.drawEllipse(pos, 4, 4);
}

void Canvas::render_shadows(QPainter& p) {
  if (scene_.obstacles().empty()) return;

  QPointF center = scene_.lamp();

  double angles[] = {0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  int alpha_per = 160 / kNumLamps;

  for (int i = 0; i < kNumLamps; ++i) {
    double a = 2.0 * M_PI * angles[i] / kNumLamps;
    QPointF offset(kSpread * std::cos(a), kSpread * std::sin(a));
    scene_.set_lamp(center + offset);

    Obstacle vis = scene_.compute_visibility();
    auto& verts = vis.points();
    if (verts.size() < 3) continue;

    QPainterPath fill;
    fill.moveTo(verts[0]);
    for (size_t j = 1; j < verts.size(); ++j) fill.lineTo(verts[j]);
    fill.closeSubpath();

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(140, 170, 255, alpha_per));
    p.drawPath(fill);
  }

  scene_.set_lamp(center);
}

void Canvas::mousePressEvent(QMouseEvent* ev) {
  if (mode_ != kPolygons) return;

  QPointF pos = ev->pos();

  if (ev->button() == Qt::LeftButton) {
    if (!poly_open_) {
      poly_open_ = true;
      Obstacle fresh;
      fresh.push_point(pos);
      fresh.push_point(pos);
      scene_.add_obstacle(std::move(fresh));
    } else {
      scene_.append_to_last(pos);
    }
    update();
  } else if (ev->button() == Qt::RightButton) {
    poly_open_ = false;
    update();
  }
}

void Canvas::mouseMoveEvent(QMouseEvent* ev) {
  QPointF pos = ev->pos();
  if (mode_ == kLight) {
    scene_.set_lamp(pos);
    update();
  } else if (poly_open_) {
    scene_.update_last(pos);
    update();
  }
}

void Canvas::keyPressEvent(QKeyEvent* ev) {
  if (mode_ != kLight) return;

  constexpr double step = 8.0;
  QPointF cur = scene_.lamp();

  switch (ev->key()) {
    case Qt::Key_W: cur.setY(cur.y() - step); break;
    case Qt::Key_A: cur.setX(cur.x() - step); break;
    case Qt::Key_S: cur.setY(cur.y() + step); break;
    case Qt::Key_D: cur.setX(cur.x() + step); break;
    default: QWidget::keyPressEvent(ev); return;
  }

  scene_.set_lamp(cur);
  update();
}
