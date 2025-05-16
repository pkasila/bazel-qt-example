#include "Ray.h"
#include <cmath>

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {}

QPointF Ray::GetBegin() const {
    return begin_;
}

QPointF Ray::GetEnd() const {
    return end_;
}

double Ray::GetAngle() const {
    return angle_;
}

void Ray::SetBegin(const QPointF& point) {
    begin_ = point;
}

void Ray::SetEnd(const QPointF& point) {
    end_ = point;
}

Ray Ray::Rotate(double phi) const {
    double len = std::hypot(end_.x() - begin_.x(), end_.y() - begin_.y());
    QPointF new_end(
        begin_.x() + len * 1000 * std::cos(angle_ + phi),
        begin_.y() + len * 1000 * std::sin(angle_ + phi)
        );
    return Ray(begin_, new_end, angle_ + phi);
}
