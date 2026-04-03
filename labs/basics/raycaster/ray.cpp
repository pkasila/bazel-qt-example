#include "ray.h"

Ray::Ray(const QPointF& begin, const QPointF& end)
    : begin_(begin), end_(end)  {
    angle_ = std::atan2(end_.y() - begin_.y(), end_.x() - begin_.x());
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

void Ray::CalculateAngle() {
    angle_ = std::atan2(end_.y() - begin_.y(), end_.x() - begin_.x());
}

void Ray::SetBegin(const QPointF& newBegin) {
    begin_ = newBegin;
    CalculateAngle();
}
void Ray::SetEnd(const QPointF& newEnd) {
    end_ = newEnd;
    CalculateAngle();
}

Ray Ray::Rotate(double phi) const {
    double len = std::hypot(end_.x() - begin_.x(), end_.y() - begin_.y());
    QPointF new_end(
        begin_.x() + len * 1000 * std::cos(angle_ + phi),
        begin_.y() + len * 1000 * std::sin(angle_ + phi));
    return Ray(begin_, new_end);
}

