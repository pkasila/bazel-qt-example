#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <QPointF>
#include <optional>
#include <utility>
#include "ray.h"

class Polygon
{
public:
    explicit Polygon(const std::vector<QPointF>& vertices = {});

    size_t vertexCount() const;
    bool empty() const;

    const std::vector<QPointF>& getVertices() const;
    void addVertex(const QPointF& vertex);
    void updateLastVertex(const QPointF& new_vertex);

    bool isVertex(const QPointF& point) const;
    std::optional<QPointF> intersectRay(const Ray& ray) const;
    std::vector<std::pair<QPointF, double>> getRayPoints(const QPointF& lightSource) const;

private:
    std::vector<QPointF> m_vertices;

    std::optional<QPointF> lineIntersection(const QPointF& p1, const QPointF& p2,
                                           const QPointF& p3, const QPointF& p4) const;
    bool pointOnSegment(const QPointF& p, const QPointF& a, const QPointF& b) const;
    double getAngle(const QPointF& center, const QPointF& point) const;
    static double Distance(const QPointF& p1, const QPointF& p2);

};

#endif // POLYGON_H
