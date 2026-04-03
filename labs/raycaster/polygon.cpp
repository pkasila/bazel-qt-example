#include "polygon.h"
#include <cmath>
#include <algorithm>
#include <limits>

Polygon::Polygon(const std::vector<QPoint>& vertices)
    : vertices_(vertices) {}

void Polygon::AddVertex(const QPoint& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPoint& new_vertex) {
    if (!vertices_.empty()) {
        vertices_.back() = new_vertex;
    }
}

std::optional<QPoint> Polygon::GetLineIntersection(
    const QPoint& p1, const QPoint& p2,
    const QPoint& p3, const QPoint& p4) {
    
    double x1 = p1.x(), y1 = p1.y();
    double x2 = p2.x(), y2 = p2.y();
    double x3 = p3.x(), y3 = p3.y();
    double x4 = p4.x(), y4 = p4.y();
    
    double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (std::abs(denom) < 1e-9) return std::nullopt;
    
    double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
    double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;
    
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        QPoint intersection(
            static_cast<int>(x1 + t * (x2 - x1)),
            static_cast<int>(y1 + t * (y2 - y1))
        );
        return intersection;
    }
    
    return std::nullopt;
}

std::optional<QPoint> Polygon::GetLineIntersectionStatic(
    const QPoint& p1, const QPoint& p2,
    const QPoint& p3, const QPoint& p4) {
    return GetLineIntersection(p1, p2, p3, p4);
}

std::optional<QPoint> Polygon::IntersectRay(const Ray& ray) const {
    if (vertices_.size() < 3) return std::nullopt;
    
    QPoint rayStart = ray.getBegin();
    QPoint rayEnd = ray.getEnd();
    
    double dx = rayEnd.x() - rayStart.x();
    double dy = rayEnd.y() - rayStart.y();
    double len = std::sqrt(dx * dx + dy * dy);
    
    if (len < 1e-9) return std::nullopt;
    
    double ndx = dx / len;
    double ndy = dy / len;
    
    QPoint farEnd(
        rayStart.x() + static_cast<int>(ndx * 10000),
        rayStart.y() + static_cast<int>(ndy * 10000)
    );
    
    std::optional<QPoint> closestIntersection;
    double minDistance = std::numeric_limits<double>::max();
    
    for (size_t i = 0; i < vertices_.size(); i++) {
        const QPoint& a = vertices_[i];
        const QPoint& b = vertices_[(i + 1) % vertices_.size()];
        
        auto intersection = GetLineIntersection(rayStart, farEnd, a, b);
        
        if (intersection) {
            double distance = std::hypot(
                intersection->x() - rayStart.x(),
                intersection->y() - rayStart.y()
            );
            
            if (distance > 1e-6 && distance < minDistance) {
                minDistance = distance;
                closestIntersection = intersection;
            }
        }
    }
    
    return closestIntersection;
}