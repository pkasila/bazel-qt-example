#pragma once
#include <vector>
#include <optional>
#include <QPointF>
#include "ray.h"

class Polygon {
public:
    Polygon() = default;
    Polygon(const std::vector<QPointF>& vertices);
    const std::vector<QPointF>& getVertices() const;
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    std::optional<QPointF> IntersectRay(const Ray& ray) const;

private:
    std::vector<QPointF> m_vertices;
};