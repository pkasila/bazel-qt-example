#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"

#include <QPointF>
#include <optional>
#include <vector>

class Polygon {
   public:
    Polygon(const std::vector<QPointF>& vers);
    const std::vector<QPointF>& GetVertices() const;
    void AddVertex(const QPointF& ver);
    void UpdateLastVertex(const QPointF& new_ver);
    std::optional<QPointF> IntersectRay(const Ray& ray) const;

   private:
    std::vector<QPointF> vers_;
    std::optional<QPointF> intersectWithEdge(
        const QPointF& p1, const QPointF& p2, const Ray& ray) const;
};

#endif  // POLYGON_H
