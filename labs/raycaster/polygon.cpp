#include "polygon.h"

#include <cmath>

namespace {

double Cross(const QPointF& a, const QPointF& b) {
    return a.x() * b.y() - a.y() * b.x();
}

double DistanceSquared(const QPointF& a, const QPointF& b) {
    double dx = a.x() - b.x();
    double dy = a.y() - b.y();
    return dx * dx + dy * dy;
}

}

Polygon::Polygon() {}

Polygon::Polygon(const std::vector<QPointF>& vertices)
    : vertices_(vertices) {}

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

    QPointF p = ray.GetBegin();
    QPointF r = ray.GetEnd() - ray.GetBegin();

    bool found = false;
    QPointF best;
    double best_dist = 1e18;

    for (size_t i = 0; i < vertices_.size(); ++i) {
        QPointF a = vertices_[i];
        QPointF b = vertices_[(i + 1) % vertices_.size()];
        QPointF q = a;
        QPointF s = b - a;

        double rxs = Cross(r, s);

        if (std::abs(rxs) < 1e-9) {
            continue;
        }

        QPointF qp = q - p;

        double t = Cross(qp, s) / rxs;
        double u = Cross(qp, r) / rxs;

        if (t > 0.0 && u >= 0.0 && u <= 1.0) {
            QPointF intersection(
                p.x() + t * r.x(),
                p.y() + t * r.y()
                );

            double dist = DistanceSquared(p, intersection);

            if (dist < best_dist) {
                best_dist = dist;
                best = intersection;
                found = true;
            }
        }
    }

    if (found) {
        return best;
    }

    return std::nullopt;
}
