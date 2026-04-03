//
// Created by blalex on 4/6/25.
//

#ifndef POLYGON_H
#define POLYGON_H
#include <QPointF>
#include <optional>
#include <vector>

#include "ray.h"

class Polygon {
public:
    Polygon() = default;
    explicit Polygon(const std::vector<QPointF>& vertices);

    [[nodiscard]] std::vector<QPointF> GetVertices() const;

    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    std::optional<QPointF> IntersectRay(const Ray& ray);
private:
    std::vector<QPointF> vertices_{};

};

#endif  // POLYGON_H
