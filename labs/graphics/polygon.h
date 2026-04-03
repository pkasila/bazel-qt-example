#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"

#include <QPoint>
#include <QPointF>
#include <vector>
#include <optional>

class Polygon {
public:
    Polygon() = default;
    Polygon(const std::vector<QPointF>& vertices);
    [[nodiscard]] int GetVerticesNumber() const;
    [[nodiscard]] QPointF GetVertex(int index) const;
    //QPoint& GetVertex(int index);
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    std::optional<QPointF> IntersectRay(const Ray& ray) const;
    const QPointF* Data();
private:
    std::vector<QPointF>vertices_;
};

#endif  // POLYGON_H
