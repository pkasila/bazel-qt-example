#pragma once

#include "ray.h"

#include <QPointF>

#include <vector>

class Polygon {
public:
    Polygon() = default;

    Polygon(const std::vector<QPointF>& vertices);

    void AddVertex(const QPointF& vertex);

    void UpdateLastVertex(const QPointF& vertex);

    QPointF IntersectRay(const Ray& ray) const;

    const std::vector<QPointF>& GetVertices() const;

    std::vector<QPointF>& GetVertices();

private:
    std::vector<QPointF> vertices_;
};
