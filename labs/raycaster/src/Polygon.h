#ifndef POLYGON_H
#define POLYGON_H
//Standard libraries
#include <QVector>
#include <QPointF>
#include <optional>
#include <cmath>
//Custom libraries
#include "GeometryUtils.h"
#include "Ray.h"
#include "Vector2D.h"
#include "Segment.h"

class Polygon {
public:
    //Constructors
    Polygon(const QVector<QPointF>& vertices);
    Polygon();
    //Getters
    const QVector<QPointF>& getVertices() const;
    QPointF getLastVertex() const;
    QVector<Segment> getSides() const noexcept;
    //Public methods
    void addVertex(const QPointF& newVertex);
    bool isPointPolygonVertex(const QPointF& point) const;
    std::optional<QPointF> intersectRay (const Ray& ray) const;

private:
    QVector<QPointF> vertices;
};

#endif // POLYGON_H
