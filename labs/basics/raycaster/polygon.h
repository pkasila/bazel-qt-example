#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <optional>
#include <QPointF>
#include "ray.h"

class Polygon {
public:
    Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {}

    const std::vector<QPointF>& getVertices() const { return vertices_; }

    void AddVertex(const QPointF& vertex) {
        vertices_.push_back(vertex);
    }

    void UpdateLastVertex(const QPointF& new_vertex) {
        if (!vertices_.empty()) {
            vertices_.back() = new_vertex;
        }
    }

    std::optional<QPointF> IntersectRay(const Ray& ray) const;

private:
    std::vector<QPointF> vertices_;
};

#endif