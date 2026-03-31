#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <QPointF>
#include <optional>
#include "ray.h"

class Polygon
{
public:
    explicit Polygon(const std::vector<QPointF>& vertices = {});

    const std::vector<QPointF>& getVertices() const;
    void addVertex(const QPointF& vertex);
    void updateLastVertex(const QPointF& new_vertex);

    size_t vertexCount() const;
    bool isEmpty() const;

    std::optional<QPointF> intersectRay(const Ray& ray) const;

private:
    std::vector<QPointF> m_vertices;

    std::optional<QPointF> segmentIntersection(const QPointF& seg1_p1, const QPointF& seg1_p2,
                                               const QPointF& seg2_p3, const QPointF& seg2_p4) const;
};

#endif // POLYGON_H

