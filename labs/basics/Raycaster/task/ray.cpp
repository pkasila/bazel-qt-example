#include "ray.h"
#include <cmath>

Ray::Ray(const QPoint& begin, const QPoint& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {
}

void Ray::SetBegin(const QPoint& begin) {
    begin_ = begin;
}

void Ray::SetEnd(const QPoint& end) {
    end_ = end;
}

void Ray::SetAngle(double angle) {
    angle_ = angle;
}

[[nodiscard]] QPoint Ray::GetBegin() const {
    return begin_;
}

[[nodiscard]] QPoint Ray::GetEnd() const {
    return end_;
}

[[nodiscard]] double Ray::GetAngle() const {
    return angle_;
}

[[nodiscard]] Ray Ray::Rotate(double angle) const {
    QPoint new_end = {end_.x() - begin_.x(), end_.y() - begin_.y()};
    DefaultRotate(new_end, angle);
    new_end.rx() += begin_.x();
    new_end.ry() += begin_.y();
    return {begin_, new_end, angle_ + angle};
}

void Ray::DefaultRotate(QPoint& point, double angle) {
    int& rx = point.rx();
    int& ry = point.ry();
    int x = point.x();
    int y = point.y();
    rx = static_cast<int>(
        (static_cast<double>(x) * cos(angle)) - (static_cast<double>(y) * sin(angle)));
    ry = static_cast<int>(
        (static_cast<double>(x) * sin(angle)) + (static_cast<double>(y) * cos(angle)));
}
