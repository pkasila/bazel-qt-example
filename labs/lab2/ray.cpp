#include "ray.h"

#include <cmath>

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {
}

QPointF Ray::GetBegin() const {
  return begin_;
}

QPointF Ray::GetEnd() const {
  return end_;
}

double Ray::GetAngle() const {
  return angle_;
}

void Ray::SetBegin(const QPointF& begin) {
  begin_ = begin;
}

void Ray::SetEnd(const QPointF& end) {
  end_ = end;
}

void Ray::SetAngle(double angle) {
  angle_ = angle;
}

Ray Ray::Rotate(double delta) const {
  double new_angle = angle_ + delta;
  double dx = end_.x() - begin_.x();
  double dy = end_.y() - begin_.y();
  double length = std::sqrt(dx * dx + dy * dy);

  double new_x = begin_.x() + length * std::cos(new_angle);
  double new_y = begin_.y() + length * std::sin(new_angle);

  return Ray(begin_, QPointF(new_x, new_y), new_angle);
}
