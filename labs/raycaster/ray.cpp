#include "ray.h"

#include <cmath>
#include <numbers>

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

Ray Ray::Rotate(double angle) const {
    double current_angle = angle_ + angle;
    double len = Length();
    QPointF new_end(
        begin_.x() + std::cos(current_angle) * len, begin_.y() + std::sin(current_angle) * len);
    return Ray(begin_, new_end, current_angle);
}

double Ray::Length() const {
    double dx = end_.x() - begin_.x();
    double dy = end_.y() - begin_.y();
    return std::sqrt(dx * dx + dy * dy);
}
