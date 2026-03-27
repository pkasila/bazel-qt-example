#pragma once

#include <vector>
#include <optional>
#include <QPointF>

class Ray;

class Polygon {
public:
    Polygon() = default;
    explicit Polygon(std::vector<QPointF> vertices);

    const std::vector<QPointF>& getVertices() const { return vertices_; }
    bool isEmpty() const { return vertices_.empty(); }
    size_t vertexCount() const { return vertices_.size(); }

    void addVertex(const QPointF& vertex);
    void updateLastVertex(const QPointF& new_vertex);
    void clear() { vertices_.clear(); }

    std::optional<QPointF> intersectRay(const Ray& ray) const;

    bool containsPoint(const QPointF& point) const;
    bool intersects(const Polygon& other) const;

private:
    std::vector<QPointF> vertices_;

    std::optional<QPointF> intersectSegments(const QPointF& p1, const QPointF& p2,
                                             const QPointF& p3, const QPointF& p4) const;
};