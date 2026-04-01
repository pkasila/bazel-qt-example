#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include "polygon.h"
#include "ray.h"

class Controller {
public:
    Controller();

    const std::vector<Polygon>& GetPolygons() const { return polygons_; }
    void AddPolygon(const Polygon& polygon) { polygons_.push_back(polygon); }
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    QPointF getLightSource() const { return light_source_; }
    void setLightSource(const QPointF& source);
    const std::vector<QPointF>& getLightSources() const { return light_sources_; }

    std::vector<Ray> CastRays();
    void IntersectRays(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    Polygon CreateLightArea(const QPointF& source);

private:
    std::vector<Polygon> polygons_;
    QPointF light_source_;
    std::vector<QPointF> light_sources_;
};

#endif