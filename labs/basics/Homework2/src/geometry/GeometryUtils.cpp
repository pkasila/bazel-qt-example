#include "GeometryUtils.h"

#include <cmath>

namespace geometry {

double Cross(const QPointF& a, const QPointF& b) {
    return a.x() * b.y() - a.y() * b.x();
}

double Dot(const QPointF& a, const QPointF& b) {
    return a.x() * b.x() + a.y() * b.y();
}

double LengthSquared(const QPointF& v) {
    return Dot(v, v);
}

double Length(const QPointF& v) {
    return std::sqrt(LengthSquared(v));
}

double DistanceSquared(const QPointF& a, const QPointF& b) {
    return LengthSquared(a - b);
}

double Distance(const QPointF& a, const QPointF& b) {
    return std::sqrt(DistanceSquared(a, b));
}

double AngleOf(const QPointF& v) {
    return std::atan2(v.y(), v.x());
}

QPointF RotateVector(const QPointF& v, double angle) {
    const double c = std::cos(angle);
    const double s = std::sin(angle);
    return QPointF(
        v.x() * c - v.y() * s,
        v.x() * s + v.y() * c
    );
}

bool AlmostEqual(double a, double b, double eps) {
    return std::abs(a - b) <= eps;
}

std::optional<QPointF> IntersectRaySegment(
    const QPointF& ray_begin,
    const QPointF& ray_dir,
    const QPointF& p1,
    const QPointF& p2,
    double eps) {

    const QPointF A = ray_begin;
    const QPointF d = ray_dir;

    const QPointF P = p1;
    const QPointF e = p2 - p1;

    const QPointF diff = P - A;
    const double denom = Cross(d, e);

    if (std::abs(denom) < eps) {
        return std::nullopt;
    }

    const double t = Cross(diff, e) / denom;
    const double u = Cross(diff, d) / denom;

    if (t < -eps) {
        return std::nullopt;
    }

    if (u < -eps || u > 1.0 + eps) {
        return std::nullopt;
    }

    return A + d * t;
}

} 