#include "ray.h"

Ray::Ray(const QPoint& begin, const QPoint& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {
}

Ray Ray::Rotate(double angle) const {
    double dx = end_.x() - begin_.x();
    double dy = end_.y() - begin_.y();

    double cos_angle = std::cos(angle);
    double sin_angle = std::sin(angle);
    
    double new_dx = dx * cos_angle - dy * sin_angle;
    double new_dy = dx * sin_angle + dy * cos_angle;
    
    QPoint new_end(begin_.x() + new_dx, begin_.y() + new_dy);
    
    return Ray(begin_, new_end, angle_ + angle);
}
