#include "ray.h"

Ray::Ray(const QPoint& begin, const QPoint& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {}

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {}

double Ray::length() const {
    double dx = end_.x() - begin_.x();
    double dy = end_.y() - begin_.y();
    return std::hypot(dx, dy);
}

Ray Ray::Rotate(double delta_angle) const {
    double new_angle = angle_ + delta_angle;
    double len = length();
    QPointF new_end(
        begin_.x() + len * std::cos(new_angle),
        begin_.y() + len * std::sin(new_angle)
    );
    return Ray(begin_, new_end, new_angle);
}