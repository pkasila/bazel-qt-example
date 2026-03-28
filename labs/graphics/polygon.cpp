#include "polygon.h"

#include <cmath>
#include <limits>

Polygon::Polygon(const std::vector<QPointF>& vers) : vers_(vers) {
}

const std::vector<QPointF>& Polygon::GetVertices() const {
    return vers_;
}

void Polygon::AddVertex(const QPointF& ver) {
    vers_.push_back(ver);
}

void Polygon::UpdateLastVertex(const QPointF& new_ver) {
    if (!vers_.empty()) {
        vers_.back() = new_ver;
    }
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (vers_.size() < 2) {
        return std::nullopt;
    }

    std::optional<QPointF> near;
    double min_dst = std::numeric_limits<double>::max();
    const QPointF& ray_begin = ray.GetBegin();
    const QPointF& ray_end = ray.GetEnd();
    QPointF ray_dir = ray_end - ray_begin;
    for (size_t i = 0; i < vers_.size(); ++i) {
        const QPointF& p1 = vers_[i];
        const QPointF& p2 = vers_[(i + 1) % vers_.size()];

        auto inter = intersectWithEdge(p1, p2, ray);
        if (inter) {
            double dst = QPointF::dotProduct(*inter - ray_begin, ray_dir);
            if (dst > 0 && dst < min_dst) {
                min_dst = dst;
                near = inter;
            }
        }
    }
    return near;
}

std::optional<QPointF> Polygon::intersectWithEdge(
    const QPointF& p1, const QPointF& p2, const Ray& ray) const {
    const QPointF& r1 = ray.GetBegin();
    const QPointF& r2 = ray.GetEnd();
    QPointF edge_vec = p2 - p1;
    QPointF ray_vec = r2 - r1;
    double det = edge_vec.x() * ray_vec.y() - edge_vec.y() * ray_vec.x();
    if (std::abs(det) < 1e-10) {
        return std::nullopt;
    }

    QPointF diff = r1 - p1;
    double t = (diff.x() * ray_vec.y() - diff.y() * ray_vec.x()) / det;
    double u = (diff.x() * edge_vec.y() - diff.y() * edge_vec.x()) / det;
    if (t >= 0.0 && t <= 1.0 && u >= 0.0) {
        return p1 + t * edge_vec;
    }
    return std::nullopt;
}
