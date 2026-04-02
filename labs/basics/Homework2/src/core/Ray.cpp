#include "Ray.h"
#include "geometry/GeometryUtils.h"

#include <cmath>

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {
}
Ray::Ray(const Ray& ray) : begin_(ray.begin_), end_(ray.end_), angle_(ray.angle_) {}

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
    angle_ = SolveAngle();
}

void Ray::SetEnd(const QPointF& end) {
    end_ = end;
    angle_ = SolveAngle();
}

void Ray::SetAngle(double angle) {
    Ray new_ray = Rotate(angle - angle_);
    *this = new_ray;
}

Ray Ray::Rotate(double angle) const {
    QPointF direction = end_ - begin_;
    QPointF new_end = geometry::RotateVector(direction, angle);

    return Ray(begin_, begin_ + new_end, angle_ + angle);

}

double Ray::SolveAngle() const {
    return geometry::AngleOf(end_ - begin_);
}