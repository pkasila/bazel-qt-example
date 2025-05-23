#include "ray.h"

#include <cmath>

Ray::Ray() {}

Ray::Ray(const QPointF& beginn, const QPointF& end, double angle) {
    begin = beginn;
    double x1 = begin.x();
    double y1 = begin.y();
    double x2 = end.x();
    double y2 = end.y();

    double x = x2 - x1;
    double y = y2 - y1;

    x_dir = x * cos(angle) - y * sin(angle);
    y_dir = x * sin(angle) + y * cos(angle);

    t = 100000000;
}

Ray Ray::Rotate(double angle) const {
    // Повернуть текущий вектор направления на угол
    double rotated_x = x_dir * cos(angle) - y_dir * sin(angle);
    double rotated_y = x_dir * sin(angle) + y_dir * cos(angle);
    QPointF end_rotated(begin.x() + rotated_x, begin.y() + rotated_y);
    return Ray(begin, end_rotated, 0); // Уже повернут, угол = 0
}

QPointF Ray::GetBegin() const {
    return begin;
}

double Ray::GetXDir() const {
    return x_dir;
}

double Ray::GetYDir() const {
    return y_dir;
}

double Ray::GetBeginX() const {
    return begin.x();
}

double Ray::GetBeginY() const {
    return begin.y();
}

double Ray::GetT() const {
    return t;
}

void Ray::SetT(double tt){
    t = tt;
}

QPointF Ray::MakeEnd() const {
    return QPointF(begin.x() + t * x_dir, begin.y() + t * y_dir);
}
