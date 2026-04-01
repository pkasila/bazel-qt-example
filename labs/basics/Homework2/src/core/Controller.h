#pragma once

#include <QPointF>
#include <vector>

#include "Polygon.h"
#include "Ray.h"

class Controller {
public:
    const std::vector<Polygon>& GetPolygons() const;

    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    const QPointF& GetLightSource() const;
    void SetLightSource(const QPointF& light_source);

    std::vector<Ray> CastRays() const;
    void IntersectRays(std::vector<Ray>* rays) const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;
    Polygon CreateLightArea() const;

private:
    std::vector<Polygon> polygons_;
    QPointF light_source_;
};