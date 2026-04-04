#include "labs/raycaster/polygon.h"

#include <cmath>
#include <limits>

namespace {

constexpr double kIntersectionEpsilon = 1e-7;

double Cross(const QPointF& lhs, const QPointF& rhs) {
    return lhs.x() * rhs.y() - lhs.y() * rhs.x();
}

double Dot(const QPointF& lhs, const QPointF& rhs) {
    return lhs.x() * rhs.x() + lhs.y() * rhs.y();
}

double SquaredLength(const QPointF& vector) {
    return vector.x() * vector.x() + vector.y() * vector.y();
}

std::optional<QPointF> IntersectRayWithSegment(
    const Ray& ray,
    const QPointF& segment_begin,
    const QPointF& segment_end
) {
    const QPointF ray_direction = ray.GetEnd() - ray.GetBegin();
    const double ray_length_squared = SquaredLength(ray_direction);
    if (ray_length_squared < kIntersectionEpsilon) {
        return std::nullopt;
    }

    const QPointF segment_direction = segment_end - segment_begin;
    const double denominator = Cross(ray_direction, segment_direction);
    const QPointF delta = segment_begin - ray.GetBegin();
    if (std::abs(denominator) < kIntersectionEpsilon) {
        if (std::abs(Cross(delta, ray_direction)) >= kIntersectionEpsilon) {
            return std::nullopt;
        }

        const double projection_begin = Dot(segment_begin - ray.GetBegin(), ray_direction) /
                                        ray_length_squared;
        const double projection_end = Dot(segment_end - ray.GetBegin(), ray_direction) /
                                      ray_length_squared;
        const double projection_min = std::min(projection_begin, projection_end);
        const double projection_max = std::max(projection_begin, projection_end);
        if (projection_max <= kIntersectionEpsilon) {
            return std::nullopt;
        }

        const double nearest_projection = projection_min > kIntersectionEpsilon
                                              ? projection_min
                                              : kIntersectionEpsilon;
        if (nearest_projection > projection_max + kIntersectionEpsilon) {
            return std::nullopt;
        }

        return ray.GetBegin() + ray_direction * nearest_projection;
    }

    const double ray_parameter = Cross(delta, segment_direction) / denominator;
    const double segment_parameter = Cross(delta, ray_direction) / denominator;
    if (ray_parameter <= kIntersectionEpsilon ||
        segment_parameter < -kIntersectionEpsilon ||
        segment_parameter > 1.0 + kIntersectionEpsilon) {
        return std::nullopt;
    }

    return ray.GetBegin() + ray_direction * ray_parameter;
}

}  // namespace

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {
}

const std::vector<QPointF>& Polygon::GetVertices() const {
    return vertices_;
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (vertices_.empty()) {
        return;
    }
    vertices_.back() = new_vertex;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (vertices_.size() < 2) {
        return std::nullopt;
    }

    std::optional<QPointF> closest_intersection;
    double closest_distance = std::numeric_limits<double>::max();

    for (std::size_t index = 0; index < vertices_.size(); ++index) {
        const QPointF& edge_begin = vertices_[index];
        const QPointF& edge_end = vertices_[(index + 1) % vertices_.size()];
        if (SquaredLength(edge_end - edge_begin) < kIntersectionEpsilon) {
            continue;
        }

        const std::optional<QPointF> intersection =
            IntersectRayWithSegment(ray, edge_begin, edge_end);
        if (!intersection.has_value()) {
            continue;
        }

        const double distance = SquaredLength(*intersection - ray.GetBegin());
        if (distance < closest_distance) {
            closest_distance = distance;
            closest_intersection = intersection;
        }
    }

    return closest_intersection;
}
