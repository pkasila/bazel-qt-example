#include "misc.h"

#include <cmath>

double misc::DotProduct(const QPointF& v1, const QPointF& v2) {
    return (v1.x() * v2.x()) + (v1.y() * v2.y());
}

double misc::Length(const QPointF& p1, const QPointF& p2) {
    return std::sqrt(DotProduct(p2 - p1, p2 - p1));
}

double misc::Length(const QPointF& v1) {
    return std::sqrt(DotProduct(v1, v1));
}

double misc::CrossProductValue(const QPointF& v1, const QPointF& v2) {
    return (v1.x() * v2.y()) - (v1.y() * v2.x());
}

double misc::Angle(const QPointF& v1) {
    double angle = std::atan2(v1.y(), v1.x());
    if (angle < 0) {
        angle += 2 * M_PI;
    }
    return angle;
}

bool misc::IsCollinear(const QPointF& v1, const QPointF& v2) {
    return std::abs(CrossProductValue(v1, v2)) < kEPS2;
}