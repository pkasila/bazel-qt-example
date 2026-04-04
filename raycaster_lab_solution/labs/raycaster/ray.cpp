#include "labs/raycaster/ray.h"

#include "labs/raycaster/geometry_utils.h"

namespace raycaster {

Ray::Ray() = default;

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
    const QPointF rotated_vector = geometry::RotateVector(end_ - begin_, angle);
    return Ray(begin_, begin_ + rotated_vector, angle_ + angle);
}

}  // namespace raycaster
