//
// Created by Zazerg on 3/31/25.
//

#ifndef POLYGON_H
#define POLYGON_H
#include <qvariant.h>
#include <vector>

#include "Ray.h"


class Polygon {
public:
    Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {};
    Polygon() : Polygon(std::vector<QPointF>()) {};
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& vertex);

    std::optional<QPointF> Intersect(Ray& ray) const;

    const std::vector<QPointF>& getVertices() const;

    int length() const;

    bool isPointInside(const QPointF& point) const;

private:
    std::vector<QPointF> vertices_;
};



#endif //POLYGON_H
