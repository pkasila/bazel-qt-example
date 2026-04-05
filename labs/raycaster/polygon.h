#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <optional>
#include "ray.h"

class Polygon {
public:
    Polygon() = default;
    Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {}
    
    const std::vector<QPointF>& getVertices() const { return vertices_; }
    void addVertex(const QPointF& vertex) { vertices_.push_back(vertex); }
    void updateLastVertex(const QPointF& vertex) { if(!vertices_.empty()) vertices_.back() = vertex; }
    
    std::optional<QPointF> intersectRay(const Ray& ray) const;

private:
    std::vector<QPointF> vertices_;
};

#endif