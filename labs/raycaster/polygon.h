#pragma once

#include <vector>
#include <optional>
#include <QPoint>
#include "ray.h"

class Polygon {
public:
    Polygon() = default;
    Polygon(const std::vector<QPoint>& vertices);
    
    const std::vector<QPoint>& getVertices() const { return vertices_; }
    
    void AddVertex(const QPoint& vertex);
    void UpdateLastVertex(const QPoint& new_vertex);
    std::optional<QPoint> IntersectRay(const Ray& ray) const;
    
    bool IsClosed() const { return vertices_.size() >= 3; }
    
    static std::optional<QPoint> GetLineIntersectionStatic(
        const QPoint& p1, const QPoint& p2,
        const QPoint& p3, const QPoint& p4);
    
private:
    std::vector<QPoint> vertices_;
    
    static bool IsPointOnSegment(const QPoint& p, const QPoint& a, const QPoint& b);
    static std::optional<QPoint> GetLineIntersection(
        const QPoint& p1, const QPoint& p2,
        const QPoint& p3, const QPoint& p4);
};