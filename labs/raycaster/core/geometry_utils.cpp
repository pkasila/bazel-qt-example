#include "labs/raycaster/core/geometry_utils.h"

#include <QPointF>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numbers>
#include <optional>
#include <vector>

namespace raycaster::geom {
static double Orientation(const QPointF& a, const QPointF& b, const QPointF& c) {
    return Cross(Subtract(b, a), Subtract(c, a));
}

static bool OnSegment(const QPointF& a, const QPointF& b, const QPointF& p) {
    const double min_x = std::min(a.x(), b.x()) - kIntersectionEpsilon;
    const double max_x = std::max(a.x(), b.x()) + kIntersectionEpsilon;
    const double min_y = std::min(a.y(), b.y()) - kIntersectionEpsilon;
    const double max_y = std::max(a.y(), b.y()) + kIntersectionEpsilon;
    return AlmostEqual(Orientation(a, b, p), 0.0, 1e-7) && p.x() >= min_x && p.x() <= max_x &&
           p.y() >= min_y && p.y() <= max_y;
}

QPointF Add(const QPointF& lhs, const QPointF& rhs) {
    return {lhs.x() + rhs.x(), lhs.y() + rhs.y()};
}

QPointF Subtract(const QPointF& lhs, const QPointF& rhs) {
    return {lhs.x() - rhs.x(), lhs.y() - rhs.y()};
}

QPointF Multiply(const QPointF& point, double scalar) {
    return {point.x() * scalar, point.y() * scalar};
}

double Dot(const QPointF& lhs, const QPointF& rhs) {
    return (lhs.x() * rhs.x()) + (lhs.y() * rhs.y());
}

double Cross(const QPointF& lhs, const QPointF& rhs) {
    return (lhs.x() * rhs.y()) - (lhs.y() * rhs.x());
}

double LengthSquared(const QPointF& vector) {
    return Dot(vector, vector);
}

double DistanceSquared(const QPointF& lhs, const QPointF& rhs) {
    return LengthSquared(Subtract(lhs, rhs));
}

double Distance(const QPointF& lhs, const QPointF& rhs) {
    return std::sqrt(DistanceSquared(lhs, rhs));
}

bool AlmostEqual(double lhs, double rhs, double epsilon) {
    return std::abs(lhs - rhs) <= epsilon;
}

bool AlmostSamePoint(const QPointF& lhs, const QPointF& rhs, double epsilon) {
    return Distance(lhs, rhs) <= epsilon;
}

double NormalizeAngle(double angle) {
    constexpr double kTwoPi = 2.0 * std::numbers::pi_v<double>;
    while (angle < 0.0) {
        angle += kTwoPi;
    }
    while (angle >= kTwoPi) {
        angle -= kTwoPi;
    }
    return angle;
}

std::optional<QPointF> IntersectRaySegment(
    const QPointF& ray_origin, const QPointF& ray_direction, const QPointF& segment_begin,
    const QPointF& segment_end) {
    if (LengthSquared(ray_direction) <= kIntersectionEpsilon) {
        return std::nullopt;
    }

    const QPointF segment_direction = Subtract(segment_end, segment_begin);
    const double denominator = Cross(ray_direction, segment_direction);
    const QPointF origin_to_segment = Subtract(segment_begin, ray_origin);

    if (AlmostEqual(denominator, 0.0, kIntersectionEpsilon)) {
        if (!AlmostEqual(Cross(origin_to_segment, ray_direction), 0.0, 1e-7)) {
            return std::nullopt;
        }

        const double ray_length_squared = LengthSquared(ray_direction);
        const double t0 =
            Dot(Subtract(segment_begin, ray_origin), ray_direction) / ray_length_squared;
        const double t1 =
            Dot(Subtract(segment_end, ray_origin), ray_direction) / ray_length_squared;
        const double nearest_t = std::min(t0, t1);
        const double farthest_t = std::max(t0, t1);

        if (farthest_t < -kIntersectionEpsilon) {
            return std::nullopt;
        }

        const double hit_t = nearest_t >= 0.0 ? nearest_t : 0.0;
        return Add(ray_origin, Multiply(ray_direction, hit_t));
    }

    const double t = Cross(origin_to_segment, segment_direction) / denominator;
    const double u = Cross(origin_to_segment, ray_direction) / denominator;

    if (t >= -kIntersectionEpsilon && u >= -kIntersectionEpsilon &&
        u <= 1.0 + kIntersectionEpsilon) {
        return Add(ray_origin, Multiply(ray_direction, std::max(0.0, t)));
    }

    return std::nullopt;
}

bool SegmentsIntersect(
    const QPointF& a_begin, const QPointF& a_end, const QPointF& b_begin, const QPointF& b_end,
    bool include_touching) {
    const double o1 = Orientation(a_begin, a_end, b_begin);
    const double o2 = Orientation(a_begin, a_end, b_end);
    const double o3 = Orientation(b_begin, b_end, a_begin);
    const double o4 = Orientation(b_begin, b_end, a_end);

    const bool general_intersection =
        ((o1 > kIntersectionEpsilon && o2 < -kIntersectionEpsilon) ||
         (o1 < -kIntersectionEpsilon && o2 > kIntersectionEpsilon)) &&
        ((o3 > kIntersectionEpsilon && o4 < -kIntersectionEpsilon) ||
         (o3 < -kIntersectionEpsilon && o4 > kIntersectionEpsilon));

    if (general_intersection) {
        return true;
    }

    if (!include_touching) {
        return false;
    }

    return (
        OnSegment(a_begin, a_end, b_begin) || OnSegment(a_begin, a_end, b_end) ||
        OnSegment(b_begin, b_end, a_begin) || OnSegment(b_begin, b_end, a_end));
}

bool PointInPolygon(const QPointF& point, const std::vector<QPointF>& polygon) {
    if (polygon.size() < 3U) {
        return false;
    }

    bool inside = false;
    for (std::size_t i = 0; i < polygon.size(); ++i) {
        const QPointF& current = polygon[i];
        const QPointF& next = polygon[(i + 1U) % polygon.size()];

        if (OnSegment(current, next, point)) {
            return true;
        }

        const bool intersects_scanline = ((current.y() > point.y()) != (next.y() > point.y()));
        if (!intersects_scanline) {
            continue;
        }

        const double intersection_x =
            current.x() +
            (((next.x() - current.x()) * (point.y() - current.y())) / (next.y() - current.y()));

        if (intersection_x >= point.x() - kIntersectionEpsilon) {
            inside = !inside;
        }
    }

    return inside;
}

double SignedPolygonArea(const std::vector<QPointF>& polygon) {
    if (polygon.size() < 3U) {
        return 0.0;
    }

    double doubled_area = 0.0;
    for (std::size_t i = 0; i < polygon.size(); ++i) {
        const QPointF& current = polygon[i];
        const QPointF& next = polygon[(i + 1U) % polygon.size()];
        doubled_area += (current.x() * next.y()) - (next.x() * current.y());
    }

    return 0.5 * doubled_area;
}

bool HasSignificantArea(const std::vector<QPointF>& polygon, double epsilon) {
    return std::abs(SignedPolygonArea(polygon)) > epsilon;
}

bool IsSimplePolygon(const std::vector<QPointF>& polygon) {
    if (polygon.size() < 3U || !HasSignificantArea(polygon)) {
        return false;
    }

    const std::size_t n = polygon.size();
    for (std::size_t i = 0; i < n; ++i) {
        const QPointF& a_begin = polygon[i];
        const QPointF& a_end = polygon[(i + 1U) % n];

        for (std::size_t j = i + 1U; j < n; ++j) {
            const QPointF& b_begin = polygon[j];
            const QPointF& b_end = polygon[(j + 1U) % n];

            const bool share_vertex = i == j || (i + 1U) % n == j || i == (j + 1U) % n;

            if (share_vertex) {
                continue;
            }

            if (SegmentsIntersect(a_begin, a_end, b_begin, b_end, true)) {
                return false;
            }
        }
    }

    return true;
}

bool PolygonsIntersect(const std::vector<QPointF>& lhs, const std::vector<QPointF>& rhs) {
    if (lhs.size() < 3U || rhs.size() < 3U) {
        return false;
    }

    for (std::size_t i = 0; i < lhs.size(); ++i) {
        const QPointF& a_begin = lhs[i];
        const QPointF& a_end = lhs[(i + 1U) % lhs.size()];
        for (std::size_t j = 0; j < rhs.size(); ++j) {
            const QPointF& b_begin = rhs[j];
            const QPointF& b_end = rhs[(j + 1U) % rhs.size()];
            if (SegmentsIntersect(a_begin, a_end, b_begin, b_end, true)) {
                return true;
            }
        }
    }

    return PointInPolygon(lhs.front(), rhs) || PointInPolygon(rhs.front(), lhs);
}

}  // namespace raycaster::geom
