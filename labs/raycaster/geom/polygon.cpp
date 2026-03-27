#include "polygon.h"

#include "misc.h"

#include <optional>

namespace {
std::optional<QPointF> RaySegmentIntersection(
    const QPointF& begin, const QPointF& end, const Ray& ray) {
    if (misc::IsCollinear(end - begin, ray.GetEnd() - ray.GetBegin())) {
        return std::nullopt;
    }
    const double r_px = ray.GetBegin().x();
    const double r_py = ray.GetBegin().y();
    const double r_dx = ray.GetEnd().x() - ray.GetBegin().x();
    const double r_dy = ray.GetEnd().y() - ray.GetBegin().y();

    const double s_px = begin.x();
    const double s_py = begin.y();
    const double s_dx = end.x() - begin.x();
    const double s_dy = end.y() - begin.y();

    const double t2 = (r_dx * (s_py - r_py) + r_dy * (r_px - s_px)) / (s_dx * r_dy - s_dy * r_dx);
    const double t1 = (s_px + s_dx * t2 - r_px) / r_dx;

    if (t2 < 0 || t2 > 1 || t1 < 0) {
        return std::nullopt;
    }

    QPointF intersection = ray.GetBegin() + t1 * (ray.GetEnd() - ray.GetBegin());
    return intersection;
}
}  // namespace

Polygon::Polygon(std::vector<QPointF>& vertices) : vertices_(vertices) {
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
    if (vertices_.size() < 2) {
        return std::nullopt;
    }
    double min_distance = std::numeric_limits<double>::max();
    std::optional<QPointF> closest_intersection = std::nullopt;
    for (size_t i = 0; i < vertices_.size() - 1; ++i) {
        const QPointF& begin = vertices_[i];
        const QPointF& end = vertices_[i + 1];
        std::optional<QPointF> intersection = RaySegmentIntersection(begin, end, ray);
        if (intersection) {
            double distance = misc::Length(ray.GetBegin() - *intersection);
            if (distance < min_distance) {
                min_distance = distance;
                closest_intersection = intersection;
            }
        }
    }
    return closest_intersection;
}