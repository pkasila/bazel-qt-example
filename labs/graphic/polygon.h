#pragma once

#include <QPointF>

#include <optional>
#include <vector>

#include "ray.h"

class Polygon {
public:
    Polygon();
    explicit Polygon(const std::vector<QPointF>& vertices);

    const std::vector<QPointF>& GetVertices() const;
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    void RemoveLastVertex();
    std::optional<QPointF> IntersectRay(const Ray& ray) const;

private:
    std::vector<QPointF> vertices_;
};
