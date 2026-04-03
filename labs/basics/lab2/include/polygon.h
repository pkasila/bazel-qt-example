#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"
#include <QVector>
#include <cstddef>
#include <optional>
#include <QPointF>
#include <QLineF>

class Polygon {
public:
    Polygon();
    Polygon(const QVector<QPointF>& vertices);
    QPointF getPoint(size_t n) const;
    QVector<QPointF> getVertices();
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    std::optional<QPointF> IntersectRay(const Ray& ray) const;
    size_t getSize() const;
    bool isEmpty();
    void clear();
    std::optional<QPointF> LineIntersection(
        const QPointF& a1, const QPointF& a2, const QPointF& b1, const QPointF& b2) const;

private:
    QVector<QPointF> vertices_;
};

#endif // POLYGON_H
