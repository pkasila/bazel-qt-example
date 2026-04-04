#include "light_ray.h"

#include <cmath>

LightRay::LightRay(QPointF origin, QPointF target, double dir)
    : origin_(origin), target_(target), dir_(dir) {}

LightRay LightRay::rotated(double delta) const {
  double new_dir = dir_ + delta;
  while (new_dir > M_PI) new_dir -= 2.0 * M_PI;
  while (new_dir < -M_PI) new_dir += 2.0 * M_PI;

  double dx = target_.x() - origin_.x();
  double dy = target_.y() - origin_.y();
  double len = std::hypot(dx, dy);

  QPointF new_target(origin_.x() + len * std::cos(new_dir),
                     origin_.y() + len * std::sin(new_dir));
  return LightRay(origin_, new_target, new_dir);
}
