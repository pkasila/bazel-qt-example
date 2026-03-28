#include "ray.h"
#include <cmath>

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {}

QPointF Ray::getBegin() const { return begin_; }
QPointF Ray::getEnd() const { return end_; }
double Ray::getAngle() const { return angle_; }
void Ray::setEnd(const QPointF& end) { end_ = end; }

Ray Ray::rotate(double angle) const {
    double cosA = std::cos(angle);
    double sinA = std::sin(angle);
    double dx = end_.x() - begin_.x();
    double dy = end_.y() - begin_.y();
    QPointF newEnd(begin_.x() + dx * cosA - dy * sinA,
                   begin_.y() + dx * sinA + dy * cosA);
    return Ray(begin_, newEnd, angle_ + angle);
}
