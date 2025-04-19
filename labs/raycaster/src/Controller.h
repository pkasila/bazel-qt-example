#ifndef CONTROLLER_H
#define CONTROLLER_H
//Qt libraries
#include <QPointF>
#include <QVector>
#include <algorithm>
//Custom libraries
#include "Ray.h"
#include "Polygon.h"
#include "Segment.h"

class Controller {
public:
    //Constructors
    Controller(const QVector<Polygon>& polygons);
    Controller();

    //Getters
    const QVector<Polygon>& getPolygons() const;
    QPointF getLightSource() const;
    const Polygon& getLastPolygon() const;

    //Setters
    void setLightSource(const QPointF& lightSource);

    //Methods
    void addPolygon(const Polygon& polygon);
    void popPolygon();
    void addVertexToLastPolygon(const QPointF& vertex);
    QVector<Ray> castRays(const QPointF& lightSource) const;
    void intersectRays(QVector<Ray>& rays) const;
    void removeAdjacentRays(QVector<Ray>& rays, double threshold) const;
    Polygon createLightPolygon(QVector<Ray>& rays) const;

private:
    QVector<Polygon> polygons;
    QPointF lightSource;

public:
    bool isNewSideValid(const Segment& newSide) const;
    bool isNewPolygonStartingPointValid(const QPointF& point) const;
};

#endif // CONTROLLER_H
