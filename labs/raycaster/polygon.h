#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"

#include <QPointF>
#include <optional>
#include <vector>

class Polygon {
   public:
    Polygon() = default;
    Polygon(const std::vector<QPointF>& vertices);
    [[nodiscard]] const std::vector<QPointF>& GetVertices() const;
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    [[nodiscard]] std::optional<QPointF> IntersectRay(const Ray& ray) const;
    [[nodiscard]] bool IsClosed() const;

   private:
    std::vector<QPointF> vertices_;
};

#endif
