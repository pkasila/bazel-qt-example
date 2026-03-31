#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <QPointF>
#include <optional>

class Ray;

class Polygon {
public:
    Polygon(const std::vector<QPointF>& vertices = {});

    std::vector<QPointF> getVertices() const;
    void addVertex(const QPointF& vertex);
    void updateLastVertex(const QPointF& new_vertex);
    bool containsPoint(const QPointF& point) const;
    std::optional<QPointF> intersectRay(const Ray& ray) const;

private:
    std::vector<QPointF> m_vertices;

    bool isPointOnSegment(const QPointF& p, const QPointF& a, const QPointF& b) const;
    std::optional<QPointF> lineIntersection(const QPointF& a1, const QPointF& a2,
                                           const QPointF& b1, const QPointF& b2) const;
};

#endif // POLYGON_H
