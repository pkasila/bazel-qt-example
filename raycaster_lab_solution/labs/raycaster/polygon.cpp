#include "labs/raycaster/polygon.h"

#include "labs/raycaster/geometry_utils.h"
#include "labs/raycaster/ray.h"

#include <limits>

namespace raycaster {

Polygon::Polygon(const std::vector<QPointF>& vertices)
    : vertices_(vertices) {
}

const std::vector<QPointF>& Polygon::GetVertices() const {
    return vertices_;
}

std::vector<QPointF>* Polygon::MutableVertices() {
    return &vertices_;
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (vertices_.empty()) {
        vertices_.push_back(new_vertex);
        return;
    }
    vertices_.back() = new_vertex;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (vertices_.size() < 2U) {
        return std::nullopt;
    }

    const QPointF direction = ray.GetEnd() - ray.GetBegin();
    if (geometry::LengthSquared(direction) < geometry::kEpsilon) {
        return std::nullopt;
    }

    std::optional<QPointF> closest_intersection;
    double closest_distance = std::numeric_limits<double>::max();

    for (std::size_t index = 0; index < vertices_.size(); ++index) {
        const QPointF& segment_begin = vertices_[index];
        const QPointF& segment_end = vertices_[(index + 1U) % vertices_.size()];
        const auto intersection = geometry::IntersectRaySegment(
            ray.GetBegin(), direction, segment_begin, segment_end);

        if (!intersection.has_value()) {
            continue;
        }

        const double distance = geometry::Distance(ray.GetBegin(), *intersection);
        if (distance + geometry::kEpsilon < closest_distance) {
            closest_distance = distance;
            closest_intersection = intersection;
        }
    }

    return closest_intersection;
}

}  // namespace raycaster
