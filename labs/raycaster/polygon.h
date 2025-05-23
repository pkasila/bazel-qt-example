#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"
#include <vector>
#include <QPointF>

class Polygon
{
public:
    Polygon() {}
    Polygon(const std::vector<QPointF>& vertices);
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    double IntersectRay(const Ray& ray);
    void FinishPolygon();
    std::vector<QPointF>& GetVertices() {
        return vertices_;
    }

private:
    std::vector<QPointF> vertices_;
};

#endif // POLYGON_H
