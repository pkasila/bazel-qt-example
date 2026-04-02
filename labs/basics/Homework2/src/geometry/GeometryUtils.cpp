#include "GeometryUtils.h"

#include <algorithm>
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

double Orientation(const QPointF& a, const QPointF& b, const QPointF& c) {
    return Cross(b - a, c - a);
}

bool OnSegment(const QPointF& a, const QPointF& b, const QPointF& p, double eps) {
    if (std::abs(Orientation(a, b, p)) > eps) {
        return false;
    }

    return p.x() >= std::min(a.x(), b.x()) - eps &&
           p.x() <= std::max(a.x(), b.x()) + eps &&
           p.y() >= std::min(a.y(), b.y()) - eps &&
           p.y() <= std::max(a.y(), b.y()) + eps;
}

bool SegmentsIntersect(
    const QPointF& a1,
    const QPointF& a2,
    const QPointF& b1,
    const QPointF& b2,
    double eps) {

    const double o1 = Orientation(a1, a2, b1);
    const double o2 = Orientation(a1, a2, b2);
    const double o3 = Orientation(b1, b2, a1);
    const double o4 = Orientation(b1, b2, a2);

    if (((o1 > eps && o2 < -eps) || (o1 < -eps && o2 > eps)) &&
        ((o3 > eps && o4 < -eps) || (o3 < -eps && o4 > eps))) {
        return true;
    }

    if (std::abs(o1) <= eps && OnSegment(a1, a2, b1, eps)) {
        return true;
    }
    if (std::abs(o2) <= eps && OnSegment(a1, a2, b2, eps)) {
        return true;
    }
    if (std::abs(o3) <= eps && OnSegment(b1, b2, a1, eps)) {
        return true;
    }
    if (std::abs(o4) <= eps && OnSegment(b1, b2, a2, eps)) {
        return true;
    }

    return false;
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
