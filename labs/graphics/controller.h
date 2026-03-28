#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"
#include "ray.h"

#include <QPointF>
#include <QRectF>
#include <cmath>
#include <vector>

class Controller {
   public:
    const std::vector<Polygon>& GetPolygons() const;
    std::vector<Polygon>& GetPolygons();
    void AddPolygon(const Polygon& poly);
    void AddVertexToLastPolygon(const QPointF& new_ver);
    void UpdateLastPolygon(const QPointF& new_ver);
    void SetLightSources(const std::vector<QPointF>& lights);
    const std::vector<QPointF>& GetLightSources() const;
    std::vector<Ray> CastRays(const QPointF& point, const QRectF& bound) const;
    void IntersectRays(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    Polygon CreateLightArea(const QPointF& point, const QRectF& bound) const;

   private:
    std::vector<Polygon> polys_;
    std::vector<QPointF> lights_;
};

#endif  // CONTROLLER_H
