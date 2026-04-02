#pragma once

#include <QPointF>
#include <optional>
#include <vector>

#include "Ray.h"

class Polygon {
public:
    Polygon(const std::vector<QPointF>& vertices);

    const std::vector<QPointF>& GetVertices() const;

    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    void RemoveLastVertex();

    size_t VertexCount() const;
    bool Empty() const;

    std::optional<QPointF> IntersectRay(const Ray& ray) const;

    bool OnBoundary(const QPointF& point) const;
    bool ContainsPoint(const QPointF& point) const;
    bool IntersectsPolygon(const Polygon& other, bool this_closed = true) const;
    bool SelfIntersects(bool closed = true) const;

private:    
    std::vector<QPointF> vertices_;
};