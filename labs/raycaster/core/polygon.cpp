#include "labs/raycaster/core/polygon.h"

#include "labs/raycaster/core/geometry_utils.h"
#include "labs/raycaster/core/ray.h"

#include <QPointF>
#include <cstddef>
#include <limits>
#include <optional>
#include <vector>

namespace raycaster {

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {
}

const std::vector<QPointF>& Polygon::GetVertices() const {
    return vertices_;
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (!vertices_.empty()) {
        vertices_.back() = new_vertex;
    }
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (vertices_.size() < 2U) {
        return std::nullopt;
    }

    const QPointF ray_direction = geom::Subtract(ray.GetEnd(), ray.GetBegin());
    std::optional<QPointF> best_intersection;
    double best_distance = std::numeric_limits<double>::infinity();

    const std::size_t edge_count = vertices_.size();
    for (std::size_t i = 0; i < edge_count; ++i) {
        const QPointF& current = vertices_[i];
        const QPointF& next = vertices_[(i + 1U) % edge_count];

        const auto intersection =
            geom::IntersectRaySegment(ray.GetBegin(), ray_direction, current, next);
        if (!intersection.has_value()) {
            continue;
        }

        const double distance = geom::DistanceSquared(ray.GetBegin(), intersection.value());

        if (distance < best_distance &&
            !geom::AlmostSamePoint(intersection.value(), ray.GetBegin(), 1e-7)) {
            best_distance = distance;
            best_intersection = intersection;
        }
    }

    return best_intersection;
}

}  // namespace raycaster
