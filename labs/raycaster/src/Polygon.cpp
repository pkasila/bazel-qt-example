#include "Polygon.h"

#include "Ray.h"
#include "utils.h"

#include <QPointF>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <optional>
#include <vector>

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {
}

const std::vector<QPointF>& Polygon::GetVertices() const {
    return vertices_;
}

const QPointF& Polygon::GetVertex(const size_t& ind) const {
    if (ind == vertices_.size()) {
        return vertices_[0];
    }
    return vertices_.at(ind);
}

PolygonType Polygon::GetType() const {
    return type_;
}

void Polygon::SetType(PolygonType type) {
    type_ = type;
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.emplace_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& vertex) {
    vertices_.back() = vertex;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    QPointF closest;
    double min_dist = std::numeric_limits<double>::max();
    bool found = false;

    const QPointF ray_begin = ray.GetBegin();
    const QPointF ray_dir = ray.GetEnd() - ray_begin;

    for (size_t i = 0; i < vertices_.size(); ++i) {
        const QPointF v1 = GetVertex(i);
        const QPointF v2 = GetVertex(i + 1);

        auto intersection = FindIntersection(ray_begin, ray_dir, v1, v2);
        if (intersection.has_value()) {
            const double dist = std::hypot(
                intersection.value().x() - ray.GetBegin().x(),
                intersection.value().y() - ray.GetBegin().y());
            if (utils::IsLess(dist, min_dist)) {
                min_dist = dist;
                closest = intersection.value();
                found = true;
            }
        }
    }

    if (found) {
        return closest;
    }

    return std::nullopt;
}

void Polygon::Scale(const QPointF& scale) {
    for (auto& vertex : vertices_) {
        utils::Scale(vertex, scale);
    }
}

double Polygon::GetDistance(const QPointF& p) const {
    double dist = 10'000.;
    for (size_t i = 0; i < vertices_.size(); i++) {
        const QPointF& a = vertices_[i];
        const QPointF& b = GetVertex(i + 1);
        const QPointF& AB = b - a;
        const QPointF& AP = p - a;
        const double t = QPointF::dotProduct(AB, AP) / (utils::Sqr(AB.x()) + utils::Sqr(AB.y()));
        if (utils::IsLess(t, .0) || utils::IsMore(t, 1.)) {
            dist = std::min(dist, utils::GetDistance(p, a));
            dist = std::min(dist, utils::GetDistance(p, b));
        } else {
            dist = std::min(
                dist,
                (std::fabs((AB.y() * p.x()) - (AB.x() * p.y()) + b.x() * a.y() - b.y() * a.x())) /
                    utils::GetDistance(a, b));
        }
    }
    return dist;
}

bool Polygon::ContainsPoint(const QPointF& p) const {
    if (type_ == PolygonType::Creating) {
        return false;
    }

    int intersections = 0;

    for (size_t i = 0; i < vertices_.size(); i++) {
        const QPointF& a = vertices_[i];
        const QPointF& b = GetVertex(i + 1);

        if (utils::IsMore(a.y(), p.y()) != utils::IsMore(b.y(), p.y())) {
            const double x_intersect =
                a.x() + ((b.x() - a.x()) * (p.y() - a.y()) / (b.y() - a.y()));
            if (utils::IsLess(p.x(), x_intersect)) {
                intersections++;
            }
        }
    }
    return (intersections % 2) == 1;
}

std::optional<QPointF> Polygon::FindIntersection(
    const QPointF& ray_begin, const QPointF& ray_dir, const QPointF& edge_begin,
    const QPointF& edge_end) {
    const QPointF edge_dir = edge_end - edge_begin;
    const double det = (ray_dir.x() * edge_dir.y()) - (ray_dir.y() * edge_dir.x());

    if (utils::IsEqual(det, .0)) {
        return std::nullopt;
    }

    const QPointF diff = edge_begin - ray_begin;
    const double t = (diff.x() * edge_dir.y() - diff.y() * edge_dir.x()) / det;
    const double s = (diff.x() * ray_dir.y() - diff.y() * ray_dir.x()) / det;

    if (!utils::IsLess(t, .0) && !utils::IsMore(t, 1.) && !utils::IsLess(s, .0) &&
        !utils::IsMore(s, 1.)) {
        return ray_begin + ray_dir * t;
    }
    return std::nullopt;
}
