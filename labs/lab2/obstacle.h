#pragma once

#include <QPointF>
#include <optional>
#include <vector>

#include "light_ray.h"

class Obstacle {
 public:
  Obstacle() = default;
  Obstacle(std::vector<QPointF> pts);

  [[nodiscard]] const std::vector<QPointF>& points() const { return pts_; }

  void push_point(QPointF p);
  void replace_last(QPointF p);

  [[nodiscard]] std::optional<QPointF> cast(const LightRay& ray) const;

 private:
  static std::optional<QPointF> seg_intersect(
      QPointF r0, QPointF r1, QPointF s0, QPointF s1);

  std::vector<QPointF> pts_;
};
