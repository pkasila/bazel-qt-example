#pragma once

#include "ray.h"

#include <QPointF>
#include <optional>
#include <vector>

class Polygon {
public:
    Polygon();
    Polygon(const std::vector<QPointF>& vertices);

    const std::vector<QPointF>& GetVertices() const;

    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);

    std::optional<QPointF> IntersectRay(const Ray& ray) const;

private:
    std::vector<QPointF> vertices_;
};
