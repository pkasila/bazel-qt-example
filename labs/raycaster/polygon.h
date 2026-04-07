#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"

#include <QPointF>
#include <optional>

#include <vector>

class Polygon {
public:
    explicit Polygon(const std::vector<QPointF>& vertices);
    std::vector<QPointF> GetVertices() const;

    void DeleteLastVertex();
    void DeleteVertex(int index);
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);

    std::optional<QPointF> IntersectRay(const Ray& ray) const;
    static std::optional<QPointF> LineIntersection(
        const QPointF& po, const QPointF& poi, const QPointF& poin, const QPointF& point);

private:
    static double Distance(const QPointF& P1, const QPointF& P2);
    std::vector<QPointF> vertices_;
};
#endif
