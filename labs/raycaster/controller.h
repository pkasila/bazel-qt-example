#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"
#include "ray.h"
#include <vector>
#include <QPointF>

class Controller {
public:
    const std::vector<Polygon>& getPolygons() const;
    void addPolygon(const Polygon& polygon);
    void addVertexToLastPolygon(const QPointF& vertex);
    void updateLastPolygonVertex(const QPointF& new_vertex);
    QPointF getLightSource() const;
    bool setLightSource(const QPointF& pos);
    void updateBoundingPolygon(const QPointF& topLeft, const QPointF& bottomRight);
    std::vector<Ray> castRays() const;
    void intersectRays(std::vector<Ray>* rays) const;
    void removeAdjacentRays(std::vector<Ray>* rays) const;
    Polygon createLightArea() ;
    Polygon* curpol();

private:
    std::vector<Polygon> m_polygons;
    QPointF m_lightSource;
    Polygon& cur_poly =* new Polygon;

};

#endif // CONTROLLER_H
