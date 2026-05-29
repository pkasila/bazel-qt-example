#include "polygon.h"

#include <cmath>
#include <limits>

namespace {

double Cross(const QPointF& a, const QPointF& b) {
    return a.x() * b.y() - a.y() * b.x();
}

}

Polygon::Polygon(const std::vector<QPointF>& vertices)
    : vertices_(vertices) {
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& vertex) {

    if (!vertices_.empty()) {
        vertices_.back() = vertex;
    }
}

QPointF Polygon::IntersectRay(const Ray& ray) const {

    QPointF best_point = ray.GetEnd();

    double min_distance = std::numeric_limits<double>::max();

    for (size_t i = 0; i < vertices_.size(); ++i) {

        QPointF a = vertices_[i];
        QPointF b = vertices_[(i + 1) % vertices_.size()];

        QPointF r = ray.GetEnd() - ray.GetBegin();
        QPointF s = b - a;

        double denominator = Cross(r, s);

        if (std::abs(denominator) < 1e-9) {
            continue;
        }

        QPointF diff = a - ray.GetBegin();

        double t = Cross(diff, s) / denominator;
        double u = Cross(diff, r) / denominator;

        if (t > 0.0 && u >= 0.0 && u <= 1.0) {

            QPointF point = ray.GetBegin() + r * t;

            double dx = point.x() - ray.GetBegin().x();
            double dy = point.y() - ray.GetBegin().y();

            double dist = std::sqrt(dx * dx + dy * dy);

            if (dist < min_distance) {
                min_distance = dist;
                best_point = point;
            }
        }
    }

    return best_point;
}

const std::vector<QPointF>& Polygon::GetVertices() const {
    return vertices_;
}

std::vector<QPointF>& Polygon::GetVertices() {
    return vertices_;
}
