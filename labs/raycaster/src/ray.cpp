#include "ray.h"
#include <cmath>

Ray::Ray(const QPointF& begin, const QPointF& end, double angle) : begin(begin), end(end), angle(angle){}

QPointF Ray::GetBegin() const {
    return begin;
}
QPointF Ray::GetEnd() const {
    return end;
}
double Ray::GetAngle() const {
    return angle;
}

Ray Ray::Rotate(double phi) const {
    double len = std::hypot(end.x() - begin.x(), end.y() - begin.y());
    //double current_angle = std::atan2(end.x() - begin.x(), end.y() - begin.y());
    QPointF new_end(
        begin.x() + len * 1000 * std::cos(angle + phi),
        begin.y() + len * 1000 * std::sin(angle + phi)
        );
    return {begin, new_end, angle + phi};
}

void Ray::SetBegin(const QPointF& point){
    begin = point;
}

void Ray::SetEnd(const QPointF& point){
    end = point;
}


