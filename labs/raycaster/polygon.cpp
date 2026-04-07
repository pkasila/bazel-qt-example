#include "polygon.h"
#include <limits>

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {
}

std::vector<QPointF> Polygon::GetVertices() const {
    return vertices_;
}

void Polygon::DeleteLastVertex() {
    vertices_.pop_back();
}

void Polygon::DeleteVertex(const int index) {
    vertices_.erase(vertices_.begin() + index);
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (!vertices_.empty()) {
        vertices_.back() = new_vertex;
    }
}

std::optional<QPointF> Polygon::LineIntersection(
    const QPointF& P1, const QPointF& P2, const QPointF& P3, const QPointF& P4) {
    const double denom =
        (P1.x() - P2.x()) * (P3.y() - P4.y()) - (P1.y() - P2.y()) * (P3.x() - P4.x());

    if (std::abs(denom) < std::numeric_limits<double>::epsilon()) {
        return std::nullopt;
    }

    const double first =
        ((P1.x() - P3.x()) * (P3.y() - P4.y()) - (P1.y() - P3.y()) * (P3.x() - P4.x())) / denom;

    const double second =
        -((P1.x() - P2.x()) * (P1.y() - P3.y()) - (P1.y() - P2.y()) * (P1.x() - P3.x())) / denom;

    if ((first >= 0) && (first<= 1) && (second >= 0) && (second <= 1)) {
        return QPointF(P1.x() + first* (P2.x() - P1.x()), P1.y() + first * (P2.y() - P1.y()));
    }
    return std::nullopt;
}

double Polygon::Distance(const QPointF& P1, const QPointF& P2) {
    const double delx = P1.x() - P2.x();
    const double dely = P1.y() - P2.y();
    return std::sqrt((delx * delx) + (dely * dely));
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    const QPointF ray_begin = ray.GetBegin();
    const QPointF ray_end = ray.GetEnd();

    double min = std::numeric_limits<double>::max();
    QPointF near_intersec;
    bool f = false;

    for (size_t i = 0; i < vertices_.size(); ++i) {
        const QPointF P1 = vertices_[i];
        const QPointF P2 = vertices_[(i + 1) % vertices_.size()];

        auto intersec = LineIntersection(ray_begin, ray_end, P1, P2);

        if (intersec) {
            const double dist = Distance(ray_begin, *intersec);
            if (dist < min) {
                min = dist;
                near_intersec = *intersec;
                f = true;
            }
        }
    }
    return f ? std::optional(near_intersec) : std::nullopt;
}
