#include "ray.h"

#include <cmath>

Ray::Ray() : begin_(0.0, 0.0), end_(0.0, 0.0), angle_(0.0) {}

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {}

const QPointF& Ray::GetBegin() const {
    return begin_;
}

const QPointF& Ray::GetEnd() const {
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

Ray Ray::Rotate(double angle) const {
    const QPointF direction = end_ - begin_;
    const double cosine = std::cos(angle);
    const double sine = std::sin(angle);
    const QPointF rotated(
        direction.x() * cosine - direction.y() * sine,
        direction.x() * sine + direction.y() * cosine
    );
    return Ray(begin_, begin_ + rotated, angle_ + angle);
}
