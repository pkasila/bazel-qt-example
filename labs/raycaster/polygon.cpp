#include "polygon.h"

#include <algorithm>
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
    if (!vertices_.empty()) {
        vertices_.back() = new_vertex;
    }
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (vertices_.size() < 2) {
        return std::nullopt;
    }

    QPointF p1 = ray.GetBegin();
    QPointF p2 = ray.GetEnd();
    double r_dx = p2.x() - p1.x();
    double r_dy = p2.y() - p1.y();

    std::optional<QPointF> closest;
    double min_dist_sq = std::numeric_limits<double>::max();

    for (size_t i = 0; i < vertices_.size(); ++i) {
        QPointF p3 = vertices_[i];
        QPointF p4 = vertices_[(i + 1) % vertices_.size()];

        double s_dx = p4.x() - p3.x();
        double s_dy = p4.y() - p3.y();

        double den = s_dx * r_dy - s_dy * r_dx;
        if (std::abs(den) < 1e-10) {
            continue;
        }

        double u = (s_dx * (p3.y() - p1.y()) - s_dy * (p3.x() - p1.x())) / den;
        double t = (r_dx * (p3.y() - p1.y()) - r_dy * (p3.x() - p1.x())) / den;

        if (t >= -1e-7 && t <= 1.0 + 1e-7 && u >= -1e-7 && u <= 1.0 + 1e-7) {
            QPointF hit(p1.x() + u * r_dx, p1.y() + u * r_dy);
            double dx = hit.x() - p1.x();
            double dy = hit.y() - p1.y();
            double d_sq = dx * dx + dy * dy;

            if (d_sq < min_dist_sq) {
                min_dist_sq = d_sq;
                closest = hit;
            }
        }
    }

    return closest;
}

bool Polygon::IsClosed() const {
    return vertices_.size() > 2;
}
