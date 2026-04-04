#pragma once

#include <QtCore/QPointF>

#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>

namespace raycaster::geometry {

constexpr double kEpsilon = 1e-8;

inline QPointF operator+(const QPointF& lhs, const QPointF& rhs) {
    return QPointF(lhs.x() + rhs.x(), lhs.y() + rhs.y());
}

inline QPointF operator-(const QPointF& lhs, const QPointF& rhs) {
    return QPointF(lhs.x() - rhs.x(), lhs.y() - rhs.y());
}

inline QPointF operator*(const QPointF& point, double factor) {
    return QPointF(point.x() * factor, point.y() * factor);
}

inline QPointF operator*(double factor, const QPointF& point) {
    return point * factor;
}

inline QPointF operator/(const QPointF& point, double factor) {
    return QPointF(point.x() / factor, point.y() / factor);
}

inline double Dot(const QPointF& lhs, const QPointF& rhs) {
    return lhs.x() * rhs.x() + lhs.y() * rhs.y();
}

inline double Cross(const QPointF& lhs, const QPointF& rhs) {
    return lhs.x() * rhs.y() - lhs.y() * rhs.x();
}

inline double LengthSquared(const QPointF& vector) {
    return Dot(vector, vector);
}

inline double Length(const QPointF& vector) {
    return std::sqrt(LengthSquared(vector));
}

inline double DistanceSquared(const QPointF& lhs, const QPointF& rhs) {
    return LengthSquared(lhs - rhs);
}

inline double Distance(const QPointF& lhs, const QPointF& rhs) {
    return std::sqrt(DistanceSquared(lhs, rhs));
}

inline QPointF Normalize(const QPointF& vector) {
    const double length = Length(vector);
    if (length < kEpsilon) {
        return QPointF(0.0, 0.0);
    }
    return vector / length;
}

inline QPointF RotateVector(const QPointF& vector, double angle) {
    const double cosine = std::cos(angle);
    const double sine = std::sin(angle);
    return QPointF(
        vector.x() * cosine - vector.y() * sine,
        vector.x() * sine + vector.y() * cosine
    );
}

inline QPointF PointOnAngle(const QPointF& origin, double angle, double length) {
    return origin + QPointF(std::cos(angle) * length, std::sin(angle) * length);
}

inline std::optional<QPointF> IntersectRaySegment(
    const QPointF& ray_origin,
    const QPointF& ray_direction,
    const QPointF& segment_begin,
    const QPointF& segment_end) {
    const QPointF segment_direction = segment_end - segment_begin;
    const double determinant = Cross(ray_direction, segment_direction);

    if (std::abs(determinant) < kEpsilon) {
        return std::nullopt;
    }

    const QPointF origin_delta = segment_begin - ray_origin;
    const double ray_t = Cross(origin_delta, segment_direction) / determinant;
    const double segment_t = Cross(origin_delta, ray_direction) / determinant;

    if (ray_t < -kEpsilon || segment_t < -kEpsilon || segment_t > 1.0 + kEpsilon) {
        return std::nullopt;
    }

    return ray_origin + ray_direction * ray_t;
}

inline bool NearlyEqual(double lhs, double rhs, double epsilon = 1e-6) {
    return std::abs(lhs - rhs) <= epsilon;
}

inline bool NearlyEqual(
    const QPointF& lhs,
    const QPointF& rhs,
    double epsilon = 1e-3) {
    return DistanceSquared(lhs, rhs) <= epsilon * epsilon;
}

inline bool IsInsidePolygon(const QPointF& point, const std::vector<QPointF>& vertices) {
    if (vertices.size() < 3U) {
        return false;
    }

    bool inside = false;
    for (std::size_t i = 0; i < vertices.size(); ++i) {
        const QPointF& current = vertices[i];
        const QPointF& next = vertices[(i + 1U) % vertices.size()];

        const bool intersects = ((current.y() > point.y()) != (next.y() > point.y())) &&
            (point.x() < (next.x() - current.x()) * (point.y() - current.y()) /
            ((next.y() - current.y()) + kEpsilon) + current.x());
        if (intersects) {
            inside = !inside;
        }
    }

    return inside;
}

}  // namespace raycaster::geometry
