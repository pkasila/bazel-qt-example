#include "ray.h"

#include <cmath>

Ray::Ray() = default;

Ray::Ray(const QPointF& begin,
         const QPointF& end,
         double angle)
    : begin_(begin)
    , end_(end)
    , angle_(angle) {
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

    double dx = end_.x() - begin_.x();
    double dy = end_.y() - begin_.y();

    double cs = std::cos(angle);
    double sn = std::sin(angle);

    QPointF rotated(
        dx * cs - dy * sn,
        dx * sn + dy * cs
        );

    QPointF new_end(
        begin_.x() + rotated.x(),
        begin_.y() + rotated.y()
        );

    double new_angle = std::atan2(
        new_end.y() - begin_.y(),
        new_end.x() - begin_.x()
        );

    return Ray(begin_, new_end, new_angle);
}
