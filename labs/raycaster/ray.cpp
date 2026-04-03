#include "ray.h"
#include <cmath>

Ray::Ray(const QPoint& begin, const QPoint& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {}

Ray Ray::Rotate(double angle) const {
    QPoint direction = end_ - begin_;
    double len = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
    
    double newAngle = angle_ + angle;
    QPoint newEnd(
        begin_.x() + static_cast<int>(len * std::cos(newAngle)),
        begin_.y() + static_cast<int>(len * std::sin(newAngle))
    );
    
    return Ray(begin_, newEnd, newAngle);
}

double Ray::length() const {
    double dx = end_.x() - begin_.x();
    double dy = end_.y() - begin_.y();
    return std::sqrt(dx * dx + dy * dy);
}