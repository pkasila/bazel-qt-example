#pragma once

#include <vector>
#include <optional>
#include <QPoint>
#include <QPointF>
#include "ray.h"

class Polygon {
public:
    explicit Polygon(const std::vector<QPoint>& vertices = {});
    
    [[nodiscard]] const std::vector<QPointF>& getVertices() const { return vertices_; }
    void AddVertex(const QPoint& vertex);
    void UpdateLastVertex(const QPoint& new_vertex);
    
    [[nodiscard]] std::optional<QPointF> IntersectRay(const Ray& ray) const;

private:
    std::vector<QPointF> vertices_;
    
    [[nodiscard]] static std::optional<QPointF> SegmentIntersection(
        const QPointF& p1, const QPointF& p2,
        const QPointF& p3, const QPointF& p4);
    [[nodiscard]] static double Distance(const QPointF& a, const QPointF& b);
};