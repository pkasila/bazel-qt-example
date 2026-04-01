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

    std::optional<QPointF> IntersectRay(const Ray& ray) const;

private:
    std::vector<QPointF> vertices_;

    std::optional<QPointF> Intersect1Ray(const Ray& ray, const QPointF& p1, const QPointF& p2) const;
    double GetDistanceSquared(const QPointF& p1, const QPointF& p2) const;
};