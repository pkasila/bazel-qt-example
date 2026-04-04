#include "labs/raycaster/ray.h"

#include <cmath>

namespace {

QPointF RotateVector(const QPointF& vector, double angle) {
    const double cosine = std::cos(angle);
    const double sine = std::sin(angle);
    return QPointF(
        vector.x() * cosine - vector.y() * sine,
        vector.x() * sine + vector.y() * cosine
    );
}

}  // namespace

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {
}

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
    return Ray(begin_, begin_ + RotateVector(direction, angle), angle_ + angle);
}
