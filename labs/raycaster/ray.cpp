// ray.cpp
#include "ray.h"

Ray::Ray(const QPointF& begin, const QPointF& end, double angle) 
    : begin_(begin), end_(end), angle_(angle) {}

Ray Ray::rotate(double angle) const {
    double newAngle = angle_ + angle;
    // Создаем новый конец луча на большом расстоянии (например, 2000 пикселей)
    QPointF newEnd(begin_.x() + 2000 * cos(newAngle), 
                   begin_.y() + 2000 * sin(newAngle));
    return Ray(begin_, newEnd, newAngle);
}