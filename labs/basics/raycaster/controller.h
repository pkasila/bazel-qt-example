#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"
#include <QPointF>
#include <vector>

class Controller {
public:
    Controller();
    const std::vector<Polygon>& getPolygons() const;
    void addPolygon(const Polygon& polygon);
    void addVertexToLastPolygon(const QPointF& vertex);
    void updateLastPolygon(const QPointF& newVertex);
    void setLightSource(const QPointF& lightSource);
    QPointF getLightSource() const;

    std::vector<Ray> castRays() const;
    void intersectRays(std::vector<Ray>& rays) const;
    void removeAdjacentRays(std::vector<Ray>& rays) const;
    Polygon createLightArea() const;

private:
    std::vector<Polygon> polygons_;
    QPointF lightSource_;
};

#endif // CONTROLLER_H
