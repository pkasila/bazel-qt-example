#include "ray.h"

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

Ray Ray::Rotate(const double angle) const {
    const double angle2 = angle_ + angle;
    const double delx = cosl(angle2);
    const double dely = sinl(angle2);

    const double hight = std::hypot(end_.x() - begin_.x(), end_.y() - begin_.y());
    const QPointF end = {begin_.x() + hight * delx, begin_.y() - hight * dely};
    return {begin_, end, angle2};
}
