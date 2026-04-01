#include "Ray.h"

#include "utils.h"

#include <QPointF>
#include <cmath>

Ray::Ray(const QPointF& begin, const QPointF& end, const double& angle)
    : begin_(begin), end_(end), angle_(angle) {
}

void Ray::SetBegin(const QPointF& begin) {
    begin_ = begin;
}

QPointF Ray::GetBegin() const {
    return begin_;
}

void Ray::SetEnd(const QPointF& end) {
    end_ = end;
    angle_ = utils::GetAngle(begin_, end_);
}

QPointF Ray::GetEnd() const {
    return end_;
}

void Ray::SetAngle(const double& angle) {
    angle_ = angle;
}

double Ray::GetAngle() const {
    return angle_;
}

Ray Ray::Rotate(const double& angle) const {
    double new_angle = angle + angle_;
    constexpr double k_too_big_num = 10'000.;
    const QPointF end(
        begin_.x() + k_too_big_num * std::cos(new_angle),
        begin_.y() + k_too_big_num * std::sin(new_angle));
    return {begin_, end, new_angle};
}

Ray Ray::PushBegin(const double& dist) const {
    const QPointF new_begin(
        begin_.x() + (dist * std::cos(angle_)), begin_.y() + (dist * std::sin(angle_)));
    return Ray(new_begin, end_, angle_);
}
