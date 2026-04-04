#pragma once

#include <QPointF>
#include <optional>
#include <vector>

namespace raycaster::geom {

constexpr double kIntersectionEpsilon = 1e-9;
constexpr double kRotationEpsilon = 1e-4;
constexpr double kAdjacentPointDistance = 0.5;

QPointF Add(const QPointF& lhs, const QPointF& rhs);
QPointF Subtract(const QPointF& lhs, const QPointF& rhs);
QPointF Multiply(const QPointF& point, double scalar);

double Dot(const QPointF& lhs, const QPointF& rhs);
double Cross(const QPointF& lhs, const QPointF& rhs);
double LengthSquared(const QPointF& vector);
double DistanceSquared(const QPointF& lhs, const QPointF& rhs);
double Distance(const QPointF& lhs, const QPointF& rhs);

bool AlmostEqual(double lhs, double rhs, double epsilon = 1e-6);
bool AlmostSamePoint(const QPointF& lhs, const QPointF& rhs, double epsilon = 1e-3);

double NormalizeAngle(double angle);

std::optional<QPointF> IntersectRaySegment(
    const QPointF& ray_origin, const QPointF& ray_direction, const QPointF& segment_begin,
    const QPointF& segment_end);

bool SegmentsIntersect(
    const QPointF& a_begin, const QPointF& a_end, const QPointF& b_begin, const QPointF& b_end,
    bool include_touching = true);

bool PointInPolygon(const QPointF& point, const std::vector<QPointF>& polygon);

double SignedPolygonArea(const std::vector<QPointF>& polygon);
bool HasSignificantArea(const std::vector<QPointF>& polygon, double epsilon = 1e-3);

bool IsSimplePolygon(const std::vector<QPointF>& polygon);
bool PolygonsIntersect(const std::vector<QPointF>& lhs, const std::vector<QPointF>& rhs);

}  // namespace raycaster::geom
