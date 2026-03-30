
#include "Ray.h"

Ray::Ray(const QPointF &begin, const QPointF &end, double angle) {
    begin_ = begin;
    end_ = end;
    angle_ = angle;
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

void Ray::SetBegin(const QPointF &begin) {
    begin_ = begin;
}
void Ray::SetEnd(const QPointF &end) {
    end_ = end;
}
void Ray::SetAngle(double angle) {
    angle_ = angle;
}

Ray Ray::Rotate(double angle) const {
    return Ray(this->begin_, this->end_, this->angle_ + angle);
}
