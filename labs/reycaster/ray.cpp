#include "ray.h"
#include <cmath>

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {
}

Ray Ray::Rotate(double angle) const {
    double cos_a = cos(angle);
    double sin_a = sin(angle);
    
    // Вектор направления луча
    double dx = end_.x() - begin_.x();
    double dy = end_.y() - begin_.y();
    
    // Поворот вектора
    double new_dx = dx * cos_a - dy * sin_a;
    double new_dy = dx * sin_a + dy * cos_a;
    
    // Новая конечная точка
    QPointF new_end(begin_.x() + new_dx, begin_.y() + new_dy);
    
    return Ray(begin_, new_end, angle_ + angle);
}
