#ifndef POLYGON_H
#define POLYGON_H

#include <QPointF>
#include <vector>
#include <optional>
#include "ray.h"

class Polygon
{
public:
    Polygon(const std::vector<QPointF>& vertices);

    std::vector<QPointF>& GetVertices();

    const std::vector<QPointF>& GetVertices() const;
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    std::optional<QPointF> IntersectRay(const Ray& ray);

private:
    std::vector<QPointF> vertices_;
};

class Line {
public:

    Line(QPointF first, QPointF last)
        : first_(first), last_(last) {}

    bool DoesIntersectRay(const Ray& ray) const;
    QPointF FindIntersection(const Ray& ray) const;

    QPointF first_;
    QPointF last_;
};

#endif // POLYGON_H
