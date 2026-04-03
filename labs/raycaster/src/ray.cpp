//
// Created by blalex on 4/6/25.
//

#include "ray.h"

#include <cmath>
#include <QPointF>

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {
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

void Ray::SetBegin(const QPointF& begin) {
    begin_ = begin;
}

void Ray::SetEnd(const QPointF& end) {
    end_ = end;
}

void Ray::SetAngle(const double angle) {
    angle_ = angle;
}

Ray Ray::Rotate(const double angle) const {
     const double new_angle = angle + angle_;
    const double len = length();
    const double dx = len * cos(new_angle);
    const double dy = len * sin(new_angle);

    return Ray{begin_, QPointF{begin_.x() + dx, begin_.y() + dy}, angle_ + angle};
}

double Ray::length() const {
    return sqrt((begin_.x() - end_.x()) * (begin_.x() - end_.x()) + (begin_.y() - end_.y()) * (begin_.y() - end_.y()));
}