#include "polygon.h"

#include <cmath>

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {}

std::vector<QPointF>& Polygon::GetVertices() {
    return vertices_;
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

bool Line::DoesIntersectRay(const Ray& ray) const {
    const QPointF& r_begin = ray.GetBegin();
    const QPointF& r_end = ray.GetEnd();
    const QPointF& l_begin = first_;
    const QPointF& l_end = last_;

    // Compute direction vectors
    const float r_dx = r_end.x() - r_begin.x();
    const float r_dy = r_end.y() - r_begin.y();
    const float l_dx = l_end.x() - l_begin.x();
    const float l_dy = l_end.y() - l_begin.y();

    const float denominator = l_dx * r_dy - l_dy * r_dx;

    if (std::abs(denominator) < 1e-6) {
        return false;
    }

    const float T2_numerator = r_dx * (l_begin.y() - r_begin.y()) - r_dy * (l_begin.x() - r_begin.x());
    const float T2 = T2_numerator / denominator;

    if (T2 < 0.0f || T2 > 1.0f) {
        return false;
    }

    const float T1 = (l_begin.x() + l_dx * T2 - r_begin.x()) / r_dx;

    if (T1 < 0.0f) {
        return false;
    }

    return true;
}

QPointF Line::FindIntersection(const Ray& ray) const {
    const QPointF& r_begin = ray.GetBegin();
    const QPointF& r_end = ray.GetEnd();
    const QPointF& l_begin = first_;
    const QPointF& l_end = last_;

    const float r_dx = r_end.x() - r_begin.x();
    const float r_dy = r_end.y() - r_begin.y();
    const float l_dx = l_end.x() - l_begin.x();
    const float l_dy = l_end.y() - l_begin.y();

    const float denominator = l_dx * r_dy - l_dy * r_dx;
    const float T2 = (r_dx * (l_begin.y() - r_begin.y()) - r_dy * (l_begin.x() - r_begin.x())) / denominator;
    const float T1 = (l_begin.x() + l_dx * T2 - r_begin.x()) / r_dx;

    return QPointF(r_begin.x() + T1 * r_dx, r_begin.y() + T1 * r_dy);
}


std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) {
    if (vertices_.size() < 2) {
        return std::nullopt;
    }

    std::optional<QPointF> closest_intersection = std::nullopt;
    float min_distance_squared = std::numeric_limits<float>::max();
    const QPointF& ray_origin = ray.GetBegin();

    for (size_t i = 0; i < vertices_.size(); ++i) {
        const QPointF& start = vertices_[i];
        const QPointF& end = vertices_[(i + 1) % vertices_.size()];

        if (start == end) {
            continue;
        }

        Line edge(start, end);
        if (edge.DoesIntersectRay(ray)) {
            QPointF intersection = edge.FindIntersection(ray);

            if (!std::isfinite(intersection.x()) || !std::isfinite(intersection.y())) {
                continue;
            }
            float dx = intersection.x() - ray_origin.x();
            float dy = intersection.y() - ray_origin.y();
            float dist_sq = dx * dx + dy * dy;

            if (dist_sq < min_distance_squared) {
                min_distance_squared = dist_sq;
                closest_intersection = intersection;
            }
        }
    }

    return closest_intersection;
}
