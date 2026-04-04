#pragma once

#include <QPointF>

class LightRay {
 public:
  LightRay() = default;
  LightRay(QPointF origin, QPointF target, double dir);

  [[nodiscard]] QPointF origin() const { return origin_; }
  [[nodiscard]] QPointF target() const { return target_; }
  [[nodiscard]] double direction() const { return dir_; }

  void set_origin(QPointF p) { origin_ = p; }
  void set_target(QPointF p) { target_ = p; }
  void set_direction(double d) { dir_ = d; }

  [[nodiscard]] LightRay rotated(double delta) const;

 private:
  QPointF origin_;
  QPointF target_;
  double dir_ = 0.0;
};
