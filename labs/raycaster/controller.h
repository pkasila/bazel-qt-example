#pragma once

#include "polygon.h"

#include <QPointF>
#include <vector>

class Controller {
public:
    const std::vector<Polygon>& GetPolygons() const;

    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    QPointF GetLightSource() const;
    void SetLightSource(const QPointF& point);

    std::vector<Ray> CastRays() const;
    void IntersectRays(std::vector<Ray>* rays) const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;

    Polygon CreateLightArea() const;

private:
    std::vector<Polygon> polygons_;
    QPointF light_source_;
};
