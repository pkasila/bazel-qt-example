#pragma once

#include <QPointF>
#include <optional>

namespace geometry {

double Cross(const QPointF& a, const QPointF& b);
double Dot(const QPointF& a, const QPointF& b);

double LengthSquared(const QPointF& v);
double Length(const QPointF& v);

double DistanceSquared(const QPointF& a, const QPointF& b);
double Distance(const QPointF& a, const QPointF& b);

double AngleOf(const QPointF& v);
QPointF RotateVector(const QPointF& v, double angle);

bool AlmostEqual(double a, double b, double eps = 1e-9);

double Orientation(const QPointF& a, const QPointF& b, const QPointF& c);
bool OnSegment(const QPointF& a, const QPointF& b, const QPointF& p, double eps = 1e-9);
bool SegmentsIntersect(
    const QPointF& a1,
    const QPointF& a2,
    const QPointF& b1,
    const QPointF& b2,
    double eps = 1e-9);

std::optional<QPointF> IntersectRaySegment(
    const QPointF& ray_begin,
    const QPointF& ray_dir,
    const QPointF& p1,
    const QPointF& p2,
    double eps = 1e-9);

}  