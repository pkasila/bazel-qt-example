#ifndef POLYGON_H
#define POLYGON_H

#include <QPointF>
#include <vector>
#include <optional>
#include "ray.h"

class Polygon {
public:
    Polygon(const std::vector<QPointF>& vertices);
    std::vector<QPointF> getVertices() const;
    void addVertex(const QPointF& vertex);
    void updateLastVertex(const QPointF& newVertex);
    std::optional<QPointF> intersectRay(const Ray& ray) const;

private:
    static std::optional<QPointF> segmentRayIntersection(const QPointF& A, const QPointF& B,
                                                         const QPointF& rayBeg, const QPointF& rayEnd);
    static double distanceSquared(const QPointF& a, const QPointF& b);

    std::vector<QPointF> vertices_;
};

#endif // POLYGON_H
