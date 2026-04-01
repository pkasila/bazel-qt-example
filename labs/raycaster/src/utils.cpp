#include "utils.h"

#include <QPointF>
#include <cmath>

bool utils::IsEqual(const double& x, const double& y) {
    return std::fabs(x - y) <= kEps;
}

bool utils::IsEqual(const QPointF& a, const QPointF& b) {
    return IsEqual(a.x(), b.x()) && IsEqual(a.y(), b.y());
}

bool utils::IsLess(const double& x, const double& y) {
    return x - y < kEps;
}

bool utils::IsMore(const double& x, const double& y) {
    return x - y > kEps;
}

double utils::GetDistance(const QPointF& a, const QPointF& b) {
    return std::hypot(a.x() - b.x(), a.y() - b.y());
}

double utils::GetAngle(const QPointF& a, const QPointF& b) {
    return std::atan2(b.y() - a.y(), b.x() - a.x());
}

void utils::Scale(QPointF& point, const QPointF& scale) {
    point = {point.x() * scale.x(), point.y() * scale.y()};
}

double utils::Sqr(const double& x) {
    return x * x;
}
