#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"

#include <vector>
#include <QPointF>

class Polygon
{
private:
    std::vector<QPointF> vertices;
public:
    Polygon() = default;
    Polygon(const std::vector<QPointF>& vertices);
    void setVertices(std::vector<QPointF> v);
    std::vector<QPointF> getVertices();
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    std::optional<QPointF> IntersectRay(const Ray& ray);
    //std::vector<Polygon> parse();
};

#endif // POLYGON_H
