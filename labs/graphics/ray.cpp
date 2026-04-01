#include "ray.h"

#include <cmath>

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {
}

Ray Ray::Rotate(double angle) const {
    QPointF dir = end_ - begin_;
    QPointF new_dir;
    new_dir.setX(dir.x() * cos(angle) - dir.y() * sin(angle));
    new_dir.setY(dir.x() * sin(angle) + dir.y() * cos(angle));
    QPointF newEnd = begin_ + new_dir;
    return Ray(begin_, newEnd, angle_ + angle);
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
