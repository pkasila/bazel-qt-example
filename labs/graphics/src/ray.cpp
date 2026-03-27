#include "../input/ray.h"
#include <cmath>

Ray::Ray(const QPointF& begin, const QPointF& end)
    : begin_(begin), end_(end) {}

Ray::Ray(const QPointF& begin, double angle, double length)
    : begin_(begin)
{
    end_.setX(begin_.x() + length * std::cos(angle));
    end_.setY(begin_.y() + length * std::sin(angle));
}

double Ray::getAngle() const {
    const QPointF vec = getVector();
    return std::atan2(vec.y(), vec.x());
}

double Ray::getLength() const {
    return std::sqrt(distSq(begin_, end_));
}

Ray Ray::Rotate(double angle_rad) const {
    const double current_angle = getAngle();
    const double new_angle = current_angle + angle_rad;
    const double length = getLength();
    return Ray(begin_, new_angle, length);
}