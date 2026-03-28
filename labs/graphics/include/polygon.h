#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"

#include <QPointF>
#include <optional>
#include <vector>

class Polygon {
   public:
    explicit Polygon(const std::vector<QPointF>& vertices);
    [[nodiscard]] std::vector<QPointF> GetVertices() const;
    void DeleteLastVertex();
    void DeleteVertex(int index);
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    [[nodiscard]] std::optional<QPointF> IntersectRay(const Ray& ray) const;
    static std::optional<QPointF> LineIntersection(
        const QPointF& a1, const QPointF& a2, const QPointF& b1, const QPointF& b2);

   private:
    std::vector<QPointF> vertices_;
    static double Distance(const QPointF& p1, const QPointF& p2);
};
#endif  // POLYGON_H
