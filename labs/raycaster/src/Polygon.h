#pragma once

#include <QPointF>
#include <vector>
#include <optional>
#include "Ray.h"

class Polygon {
public:
    Polygon();
    Polygon(const std::vector<QPointF>& vertices);

    std::vector<QPointF> GetVertices() const;

    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    std::optional<QPointF> IntersectRay(const Ray& ray) const;

    void Complete();
    bool IsComplete() const;

private:
    std::vector<QPointF> vertices;
    bool completed = false;

    std::optional<QPointF> IntersectLine(const QPointF& p1, const QPointF& p2, const Ray& ray) const;
};
