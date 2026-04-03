//
// Created by blalex on 4/6/25.
//

#include "polygon.h"

#include <cstddef>
#include <optional>

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {
}

std::vector<QPointF> Polygon::GetVertices() const {
    return vertices_;
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    vertices_.back() = new_vertex;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) {
    const QPointF& origin = ray.GetBegin();
    const double angle = ray.GetAngle();
    const double dx = std::cos(angle);
    const double dy = std::sin(angle);

    std::optional<QPointF> closest_intersection;
    double min_distance_sq = std::numeric_limits<double>::max();

    const size_t n = vertices_.size();
    for (size_t i = 0; i < n; ++i) {
        const QPointF& p1 = vertices_[i];
        const QPointF& p2 = vertices_[(i + 1) % n];

        const double x1 = p1.x(), y1 = p1.y();
        const double x2 = p2.x(), y2 = p2.y();
        const double x3 = origin.x(), y3 = origin.y();
        const double x4 = origin.x() + dx, y4 = origin.y() + dy;

        const double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (std::abs(denom) < 1e-9) {
            continue;
        }

        const double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
        const double u = ((x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2)) / denom;

        if (t >= 0.0 && t <= 1.0 && u >= 0.0) {
            const double ix = x1 + t * (x2 - x1);
            const double iy = y1 + t * (y2 - y1);

            const double dist_sq = (ix - x3) * (ix - x3) + (iy - y3) * (iy - y3);
            if (dist_sq < min_distance_sq) {
                min_distance_sq = dist_sq;
                closest_intersection = QPointF(ix, iy);
            }
        }
    }

    return closest_intersection;
}
