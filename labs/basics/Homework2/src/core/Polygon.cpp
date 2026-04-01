#include "Polygon.h"

#include "geometry/GeometryUtils.h"

#include <cmath>
#include <limits>

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {
}

const std::vector<QPointF>& Polygon::GetVertices() const {
    return vertices_;
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    vertices_.back() = new_vertex;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (vertices_.empty() || vertices_.size() < 2) {
        return std::nullopt;
    }

    double min_d = std::numeric_limits<double>::max();
    std::optional<QPointF> min_intersection;

    for (int i = 1; i <= vertices_.size(); ++i) {
        std::optional<QPointF> intersection;
        if (i == vertices_.size()) {
            intersection = geometry::IntersectRaySegment(
                ray.GetBegin(), ray.GetEnd() - ray.GetBegin(), vertices_[i - 1], vertices_[0]);
        } else {
            intersection = geometry::IntersectRaySegment(
                ray.GetBegin(), ray.GetEnd() - ray.GetBegin(), vertices_[i - 1], vertices_[i]);
        }

        if (intersection != std::nullopt) {
            double dist = geometry::DistanceSquared(*intersection, ray.GetBegin());
            if (dist < min_d) {
                min_d = dist;
                min_intersection = intersection;
            }
        }
    }

    if (min_d != std::numeric_limits<double>::max()) {
        return min_intersection;
    }

    return std::nullopt;
}