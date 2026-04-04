#include "polygon.h"
#include <limits>

double Polygon::Distance(const QPointF& a, const QPointF& b) {
    double dx = a.x() - b.x();
    double dy = a.y() - b.y();
    return std::hypot(dx, dy);
}

Polygon::Polygon(const std::vector<QPoint>& vertices) {
    vertices_.reserve(vertices.size());
    for (const auto& v : vertices) {
        vertices_.emplace_back(v);
    }
}

void Polygon::AddVertex(const QPoint& vertex) {
    vertices_.emplace_back(vertex);
}

void Polygon::UpdateLastVertex(const QPoint& new_vertex) {
    if (!vertices_.empty()) {
        vertices_.back() = new_vertex;
    }
}

std::optional<QPointF> Polygon::SegmentIntersection(
    const QPointF& p1, const QPointF& p2,
    const QPointF& p3, const QPointF& p4) 
{
    const double eps = 1e-9;
    double denom = (p4.y() - p3.y()) * (p2.x() - p1.x()) - 
                   (p4.x() - p3.x()) * (p2.y() - p1.y());
    
    if (std::abs(denom) < eps) return std::nullopt;
    
    double ua = ((p4.x() - p3.x()) * (p1.y() - p3.y()) - 
                 (p4.y() - p3.y()) * (p1.x() - p3.x())) / denom;
    double ub = ((p2.x() - p1.x()) * (p1.y() - p3.y()) - 
                 (p2.y() - p1.y()) * (p1.x() - p3.x())) / denom;
    
    if (ua >= -eps && ub >= -eps && ub <= 1.0 + eps) {
        return QPointF(
            p1.x() + ua * (p2.x() - p1.x()),
            p1.y() + ua * (p2.y() - p1.y())
        );
    }
    return std::nullopt;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (vertices_.size() < 2) return std::nullopt;
    
    std::optional<QPointF> closest;
    double min_dist = std::numeric_limits<double>::max();
    const QPointF& ray_begin = ray.getBegin();
    const QPointF& ray_end = ray.getEnd();
    
    for (size_t i = 0; i < vertices_.size(); ++i) {
        const QPointF& p1 = vertices_[i];
        const QPointF& p2 = vertices_[(i + 1) % vertices_.size()];
        
        auto intersection = SegmentIntersection(ray_begin, ray_end, p1, p2);
        if (intersection) {
            double dist = Distance(ray_begin, *intersection);
            if (dist < min_dist - 1e-6) {
                min_dist = dist;
                closest = *intersection;
            }
        }
    }
    return closest;
}