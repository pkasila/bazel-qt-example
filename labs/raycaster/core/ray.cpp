#include "labs/raycaster/core/ray.h"

#include "labs/raycaster/core/geometry_utils.h"

#include <QPointF>
#include <cmath>

namespace raycaster {

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(geom::NormalizeAngle(angle)) {
}

const QPointF& Ray::GetBegin() const {
    return begin_;
}

void Ray::SetBegin(const QPointF& begin) {
    begin_ = begin;
}

const QPointF& Ray::GetEnd() const {
    return end_;
}

void Ray::SetEnd(const QPointF& end) {
    end_ = end;
}

double Ray::GetAngle() const {
    return angle_;
}

void Ray::SetAngle(double angle) {
    angle_ = geom::NormalizeAngle(angle);
}

Ray Ray::Rotate(double angle) const {
    const QPointF relative_end = geom::Subtract(end_, begin_);
    const double cosine = std::cos(angle);
    const double sine = std::sin(angle);

    const QPointF rotated_relative_end(
        (relative_end.x() * cosine) - (relative_end.y() * sine),
        (relative_end.x() * sine) + (relative_end.y() * cosine));

    return {begin_, geom::Add(begin_, rotated_relative_end), geom::NormalizeAngle(angle_ + angle)};
}

}  // namespace raycaster
